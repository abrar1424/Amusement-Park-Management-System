import type { Request, Response } from "express";
import { z } from "zod";
import { PaymentStatus, RideStatus } from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const usageSchema = z.object({
  qrCode: z.string().min(5),
  rideId: z.string().uuid()
});

export async function recordRideUsage(
  request: Request,
  response: Response
): Promise<void> {
  const data = usageSchema.parse(request.body);

  const ticket = await prisma.ticket.findUnique({
    where: { qrCode: data.qrCode },
    include: { customer: true }
  });

  if (!ticket || ticket.paymentStatus !== PaymentStatus.PAID) {
    throw new AppError(404, "A valid paid ticket was not found.");
  }

  const ride = await prisma.ride.findUnique({
    where: { id: data.rideId }
  });

  if (!ride) {
    throw new AppError(404, "Ride not found.");
  }

  if (ride.status !== RideStatus.ACTIVE) {
    throw new AppError(409, "Ride is not currently active.");
  }

  if (ticket.customer.age < ride.minimumAge) {
    throw new AppError(
      409,
      "Customer does not meet the minimum age requirement."
    );
  }

  const usage = await prisma.rideUsage.create({
    data: {
      customerId: ticket.customerId,
      rideId: ride.id,
      ticketId: ticket.id
    },
    include: {
      customer: { select: { id: true, name: true } },
      ride: { select: { id: true, rideName: true } }
    }
  });

  response.status(201).json({
    success: true,
    message: "Ride usage recorded successfully.",
    data: usage
  });
}

export async function listRideUsage(
  request: Request,
  response: Response
): Promise<void> {
  const usages = await prisma.rideUsage.findMany({
    where: {
      ...(request.query.rideId
        ? { rideId: String(request.query.rideId) }
        : {}),
      ...(request.query.customerId
        ? { customerId: String(request.query.customerId) }
        : {})
    },
    include: {
      customer: { select: { id: true, name: true } },
      ride: { select: { id: true, rideName: true } },
      ticket: { select: { id: true, qrCode: true } }
    },
    orderBy: { entryTime: "desc" },
    take: 200
  });

  response.json({ success: true, data: usages });
}