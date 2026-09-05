import crypto from "node:crypto";
import type { Request, Response } from "express";
import QRCode from "qrcode";
import { z } from "zod";
import {
  EntryStatus,
  PaymentMethod,
  PaymentStatus,
  TicketType
} from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const ticketPrices: Record<TicketType, number> = {
  CHILD: 500,
  ADULT: 800,
  SENIOR: 600,
  FAMILY: 2200,
  VIP: 1500
};

const bookingSchema = z.object({
  visitDate: z.coerce.date(),
  ticketType: z.nativeEnum(TicketType)
});

const paymentSchema = z.object({
  paymentMethod: z.nativeEnum(PaymentMethod)
});

const entrySchema = z.object({
  qrCode: z.string().min(5),
  gateName: z.string().min(2).max(100)
});

export async function bookTicket(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user || request.user.role !== "CUSTOMER") {
    throw new AppError(403, "Only customers can book tickets.");
  }

  const data = bookingSchema.parse(request.body);

  const ticket = await prisma.ticket.create({
    data: {
      customerId: request.user.id,
      visitDate: data.visitDate,
      ticketType: data.ticketType,
      ticketPrice: ticketPrices[data.ticketType],
      qrCode: `PARK-${crypto.randomUUID()}`
    }
  });

  response.status(201).json({
    success: true,
    message: "Ticket booked successfully.",
    data: ticket
  });
}

export async function listMyTickets(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user || request.user.role !== "CUSTOMER") {
    throw new AppError(403, "Only customers can view this resource.");
  }

  const tickets = await prisma.ticket.findMany({
    where: { customerId: request.user.id },
    include: {
      paymentTransactions: true,
      parkEntries: true
    },
    orderBy: { bookedAt: "desc" }
  });

  response.json({ success: true, data: tickets });
}

export async function payTicket(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user || request.user.role !== "CUSTOMER") {
    throw new AppError(403, "Only customers can pay for tickets.");
  }

  const data = paymentSchema.parse(request.body);

  const existingTicket = await prisma.ticket.findFirst({
    where: {
      id: String(request.params.id),
      customerId: request.user.id
    }
  });

  if (!existingTicket) {
    throw new AppError(404, "Ticket not found.");
  }

  if (existingTicket.paymentStatus === PaymentStatus.PAID) {
    throw new AppError(409, "Ticket is already paid.");
  }

  const transactionReference = `TXN-${Date.now()}-${crypto
    .randomBytes(4)
    .toString("hex")}`;

  const result = await prisma.$transaction(async (transaction) => {
    const payment = await transaction.paymentTransaction.create({
      data: {
        ticketId: existingTicket.id,
        amount: existingTicket.ticketPrice,
        paymentMethod: data.paymentMethod,
        transactionReference,
        status: PaymentStatus.PAID,
        paidAt: new Date()
      }
    });

    const ticket = await transaction.ticket.update({
      where: { id: existingTicket.id },
      data: {
        paymentStatus: PaymentStatus.PAID,
        paymentDate: new Date()
      }
    });

    return { payment, ticket };
  });

  response.json({
    success: true,
    message: "Demo payment completed successfully.",
    data: result
  });
}

export async function getTicketQrCode(
  request: Request,
  response: Response
): Promise<void> {
  const ticket = await prisma.ticket.findUnique({
    where: { id: String(request.params.id) }
  });

  if (!ticket) {
    throw new AppError(404, "Ticket not found.");
  }

  if (
    request.user?.role === "CUSTOMER" &&
    ticket.customerId !== request.user.id
  ) {
    throw new AppError(403, "You cannot access this ticket.");
  }

  const dataUrl = await QRCode.toDataURL(ticket.qrCode);

  response.json({
    success: true,
    data: {
      ticketId: ticket.id,
      qrCode: ticket.qrCode,
      qrImage: dataUrl
    }
  });
}

export async function verifyParkEntry(
  request: Request,
  response: Response
): Promise<void> {
  const data = entrySchema.parse(request.body);

  const ticket = await prisma.ticket.findUnique({
    where: { qrCode: data.qrCode },
    include: { customer: true }
  });

  if (!ticket) {
    throw new AppError(404, "QR code is invalid.");
  }

  if (ticket.paymentStatus !== PaymentStatus.PAID) {
    throw new AppError(409, "Ticket payment is not complete.");
  }

  const existingEntry = await prisma.parkEntry.findFirst({
    where: {
      ticketId: ticket.id,
      status: EntryStatus.ENTERED,
      exitTime: null
    }
  });

  if (existingEntry) {
    throw new AppError(409, "This visitor is already inside the park.");
  }

  const entry = await prisma.parkEntry.create({
    data: {
      ticketId: ticket.id,
      gateName: data.gateName,
      status: EntryStatus.ENTERED
    }
  });

  response.status(201).json({
    success: true,
    message: "Park entry verified successfully.",
    data: {
      entry,
      customer: {
        id: ticket.customer.id,
        name: ticket.customer.name
      }
    }
  });
}

export async function recordParkExit(
  request: Request,
  response: Response
): Promise<void> {
  const data = z.object({ qrCode: z.string().min(5) }).parse(request.body);

  const ticket = await prisma.ticket.findUnique({
    where: { qrCode: data.qrCode }
  });

  if (!ticket) {
    throw new AppError(404, "QR code is invalid.");
  }

  const entry = await prisma.parkEntry.findFirst({
    where: {
      ticketId: ticket.id,
      status: EntryStatus.ENTERED,
      exitTime: null
    },
    orderBy: { entryTime: "desc" }
  });

  if (!entry) {
    throw new AppError(404, "Active park entry was not found.");
  }

  const updated = await prisma.parkEntry.update({
    where: { id: entry.id },
    data: {
      status: EntryStatus.EXITED,
      exitTime: new Date()
    }
  });

  response.json({
    success: true,
    message: "Visitor exit recorded successfully.",
    data: updated
  });
}