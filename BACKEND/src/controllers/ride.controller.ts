import type { Request, Response } from "express";
import { z } from "zod";
import { RideStatus } from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const rideSchema = z.object({
  rideName: z.string().min(2).max(120),
  description: z.string().max(5000).optional(),
  zoneId: z.string().uuid(),
  capacity: z.coerce.number().int().positive(),
  minimumAge: z.coerce.number().int().min(1).max(100).default(5),
  intensityLevel: z.coerce.number().int().min(1).max(5).default(1),
  cycleDurationMinutes: z.coerce.number().int().positive().default(5),
  status: z.nativeEnum(RideStatus).default(RideStatus.ACTIVE),
  currentQueueCount: z.coerce.number().int().min(0).default(0),
  currentWaitTimeMinutes: z.coerce.number().int().min(0).default(0)
});

const operationSchema = z.object({
  status: z.nativeEnum(RideStatus).optional(),
  currentQueueCount: z.coerce.number().int().min(0).optional(),
  currentWaitTimeMinutes: z.coerce.number().int().min(0).optional()
});

export async function listRides(
  request: Request,
  response: Response
): Promise<void> {
  const status = request.query.status as RideStatus | undefined;
  const zoneId = request.query.zoneId as string | undefined;

  const rides = await prisma.ride.findMany({
    where: {
      ...(status ? { status } : {}),
      ...(zoneId ? { zoneId } : {})
    },
    include: {
      zone: true,
      _count: {
        select: {
          rideUsages: true,
          feedbacks: true,
          maintenanceRecords: true
        }
      }
    },
    orderBy: { rideName: "asc" }
  });

  response.json({ success: true, data: rides });
}

export async function getRide(
  request: Request,
  response: Response
): Promise<void> {
  const ride = await prisma.ride.findUnique({
    where: { id: String(request.params.id) },
    include: {
      zone: true,
      queueSnapshots: {
        orderBy: { recordedAt: "desc" },
        take: 10
      },
      feedbacks: {
        orderBy: { submittedAt: "desc" },
        take: 10
      }
    }
  });

  if (!ride) {
    throw new AppError(404, "Ride not found.");
  }

  response.json({ success: true, data: ride });
}

export async function createRide(
  request: Request,
  response: Response
): Promise<void> {
  const data = rideSchema.parse(request.body);

  const zone = await prisma.zone.findUnique({
    where: { id: data.zoneId }
  });

  if (!zone) {
    throw new AppError(404, "Zone not found.");
  }

  const ride = await prisma.ride.create({ data });

  response.status(201).json({
    success: true,
    message: "Ride created successfully.",
    data: ride
  });
}

export async function updateRide(
  request: Request,
  response: Response
): Promise<void> {
  const data = rideSchema.partial().parse(request.body);

  const existing = await prisma.ride.findUnique({
    where: { id: String(request.params.id) }
  });

  if (!existing) {
    throw new AppError(404, "Ride not found.");
  }

  const ride = await prisma.ride.update({
    where: { id: String(request.params.id) },
    data: {
      ...data,
      lastStatusUpdatedAt: new Date()
    }
  });

  response.json({
    success: true,
    message: "Ride updated successfully.",
    data: ride
  });
}

export async function updateRideOperation(
  request: Request,
  response: Response
): Promise<void> {
  const data = operationSchema.parse(request.body);

  const ride = await prisma.$transaction(async (transaction) => {
    const updatedRide = await transaction.ride.update({
      where: { id: String(request.params.id) },
      data: {
        ...data,
        lastStatusUpdatedAt: new Date()
      }
    });

    if (
      data.currentQueueCount !== undefined ||
      data.currentWaitTimeMinutes !== undefined
    ) {
      await transaction.rideQueueSnapshot.create({
        data: {
          rideId: updatedRide.id,
          waitingCount:
            data.currentQueueCount ??
            updatedRide.currentQueueCount,
          estimatedWaitMinutes:
            data.currentWaitTimeMinutes ??
            updatedRide.currentWaitTimeMinutes
        }
      });
    }

    return updatedRide;
  });

  response.json({
    success: true,
    message: "Ride operation updated successfully.",
    data: ride
  });
}

export async function deleteRide(
  request: Request,
  response: Response
): Promise<void> {
  await prisma.ride.delete({ where: { id: String(request.params.id) } });

  response.json({
    success: true,
    message: "Ride deleted successfully."
  });
}