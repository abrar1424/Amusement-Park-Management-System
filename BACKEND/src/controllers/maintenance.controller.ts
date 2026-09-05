import type { Request, Response } from "express";
import { z } from "zod";
import {
  MaintenanceStatus,
  RideStatus
} from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const createSchema = z.object({
  rideId: z.string().uuid(),
  issueDescription: z.string().min(5).max(5000),
  maintenanceDate: z.coerce.date().default(() => new Date())
});

const statusSchema = z.object({
  status: z.nativeEnum(MaintenanceStatus)
});

export async function createMaintenance(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user) {
    throw new AppError(401, "Authentication is required.");
  }

  const data = createSchema.parse(request.body);

  const ride = await prisma.ride.findUnique({
    where: { id: data.rideId }
  });

  if (!ride) {
    throw new AppError(404, "Ride not found.");
  }

  const staffId =
    request.user.role === "STAFF" ? request.user.id : null;

  const maintenance = await prisma.$transaction(async (transaction) => {
    const record = await transaction.maintenance.create({
      data: {
        ...data,
        staffId,
        status: MaintenanceStatus.REQUESTED
      }
    });

    await transaction.ride.update({
      where: { id: data.rideId },
      data: {
        status: RideStatus.UNDER_MAINTENANCE,
        lastStatusUpdatedAt: new Date()
      }
    });

    return record;
  });

  response.status(201).json({
    success: true,
    message: "Maintenance request created successfully.",
    data: maintenance
  });
}

export async function listMaintenance(
  _request: Request,
  response: Response
): Promise<void> {
  const records = await prisma.maintenance.findMany({
    include: {
      ride: { select: { id: true, rideName: true } },
      staff: {
        select: {
          id: true,
          name: true,
          role: true
        }
      }
    },
    orderBy: { createdAt: "desc" }
  });

  response.json({ success: true, data: records });
}

export async function updateMaintenanceStatus(
  request: Request,
  response: Response
): Promise<void> {
  const data = statusSchema.parse(request.body);

  const existing = await prisma.maintenance.findUnique({
    where: { id: String(request.params.id) }
  });

  if (!existing) {
    throw new AppError(404, "Maintenance record not found.");
  }

  const record = await prisma.$transaction(async (transaction) => {
    const updated = await transaction.maintenance.update({
      where: { id: existing.id },
      data: {
        status: data.status,
        completedAt:
          data.status === MaintenanceStatus.COMPLETED
            ? new Date()
            : null
      }
    });

    if (data.status === MaintenanceStatus.COMPLETED) {
      await transaction.ride.update({
        where: { id: existing.rideId },
        data: {
          status: RideStatus.ACTIVE,
          lastStatusUpdatedAt: new Date()
        }
      });
    }

    return updated;
  });

  response.json({
    success: true,
    message: "Maintenance status updated successfully.",
    data: record
  });
}

Backend/src/controllers/report.controller.ts

import type { Request, Response } from "express";
import {
  EntryStatus,
  MaintenanceStatus,
  PaymentStatus
} from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";

export async function revenueReport(
  _request: Request,
  response: Response
): Promise<void> {
  const rows = await prisma.$queryRaw<
    Array<{
      visit_date: Date;
      paid_ticket_count: number;
      total_revenue: number;
      average_ticket_value: number;
    }>
  >`
    SELECT
      visit_date,
      COUNT(*)::int AS paid_ticket_count,
      COALESCE(SUM(ticket_price), 0)::float8 AS total_revenue,
      COALESCE(AVG(ticket_price), 0)::float8 AS average_ticket_value
    FROM tickets
    WHERE payment_status = 'PAID'
    GROUP BY visit_date
    ORDER BY visit_date
  `;

  response.json({ success: true, data: rows });
}

export async function popularRidesReport(
  _request: Request,
  response: Response
): Promise<void> {
  const rows = await prisma.$queryRaw<
    Array<{
      ride_id: string;
      ride_name: string;
      zone_name: string;
      total_ride_entries: number;
      unique_riders: number;
    }>
  >`
    SELECT
      r.ride_id,
      r.ride_name,
      z.zone_name,
      COUNT(ru.usage_id)::int AS total_ride_entries,
      COUNT(DISTINCT ru.customer_id)::int AS unique_riders
    FROM rides r
    JOIN zones z ON z.zone_id = r.zone_id
    LEFT JOIN ride_usage ru ON ru.ride_id = r.ride_id
    GROUP BY r.ride_id, r.ride_name, z.zone_name
    ORDER BY total_ride_entries DESC, unique_riders DESC, r.ride_name
  `;

  response.json({ success: true, data: rows });
}

export async function dailyVisitorReport(
  _request: Request,
  response: Response
): Promise<void> {
  const rows = await prisma.$queryRaw<
    Array<{
      visit_day: Date;
      verified_visitors: number;
      total_gate_scans: number;
    }>
  >`
    SELECT
      DATE(entry_time) AS visit_day,
      COUNT(DISTINCT ticket_id)::int AS verified_visitors,
      COUNT(*)::int AS total_gate_scans
    FROM park_entries
    WHERE status IN ('ENTERED', 'EXITED')
    GROUP BY DATE(entry_time)
    ORDER BY visit_day
  `;

  response.json({ success: true, data: rows });
}

export async function satisfactionReport(
  _request: Request,
  response: Response
): Promise<void> {
  const rows = await prisma.$queryRaw<
    Array<{
      ride_id: string;
      ride_name: string;
      feedback_count: number;
      average_rating: number;
    }>
  >`
    SELECT
      r.ride_id,
      r.ride_name,
      COUNT(f.feedback_id)::int AS feedback_count,
      COALESCE(AVG(f.rating), 0)::float8 AS average_rating
    FROM rides r
    LEFT JOIN feedback f ON f.ride_id = r.ride_id
    GROUP BY r.ride_id, r.ride_name
    ORDER BY average_rating DESC, feedback_count DESC
  `;

  response.json({ success: true, data: rows });
}

export async function dashboardReport(
  _request: Request,
  response: Response
): Promise<void> {
  const [
    customers,
    rides,
    tickets,
    verifiedEntries,
    activeAlerts,
    openMaintenance,
    revenue
  ] = await Promise.all([
    prisma.customer.count(),
    prisma.ride.count(),
    prisma.ticket.count(),
    prisma.parkEntry.count({
      where: { status: { in: [EntryStatus.ENTERED, EntryStatus.EXITED] } }
    }),
    prisma.emergencyAlert.count({ where: { isActive: true } }),
    prisma.maintenance.count({
      where: {
        status: {
          in: [
            MaintenanceStatus.REQUESTED,
            MaintenanceStatus.ASSIGNED,
            MaintenanceStatus.IN_PROGRESS
          ]
        }
      }
    }),
    prisma.ticket.aggregate({
      where: { paymentStatus: PaymentStatus.PAID },
      _sum: { ticketPrice: true }
    })
  ]);

  response.json({
    success: true,
    data: {
      customers,
      rides,
      tickets,
      verifiedEntries,
      activeAlerts,
      openMaintenance,
      totalRevenue: Number(revenue._sum.ticketPrice ?? 0)
    }
  });
}