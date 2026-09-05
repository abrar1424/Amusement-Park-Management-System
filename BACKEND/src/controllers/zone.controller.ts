import type { Request, Response } from "express";
import { z } from "zod";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const zoneSchema = z.object({
  zoneName: z.string().min(2).max(100),
  description: z.string().max(2000).optional(),
  mapX: z.coerce.number().int().min(0).max(100).optional(),
  mapY: z.coerce.number().int().min(0).max(100).optional()
});

export async function listZones(
  _request: Request,
  response: Response
): Promise<void> {
  const zones = await prisma.zone.findMany({
    include: {
      _count: { select: { rides: true } }
    },
    orderBy: { zoneName: "asc" }
  });

  response.json({ success: true, data: zones });
}

export async function createZone(
  request: Request,
  response: Response
): Promise<void> {
  const data = zoneSchema.parse(request.body);
  const zone = await prisma.zone.create({ data });

  response.status(201).json({
    success: true,
    message: "Zone created successfully.",
    data: zone
  });
}

export async function updateZone(
  request: Request,
  response: Response
): Promise<void> {
  const data = zoneSchema.partial().parse(request.body);

  const existing = await prisma.zone.findUnique({
    where: { id: String(request.params.id) }
  });

  if (!existing) {
    throw new AppError(404, "Zone not found.");
  }

  const zone = await prisma.zone.update({
    where: { id: String(request.params.id) },
    data
  });

  response.json({
    success: true,
    message: "Zone updated successfully.",
    data: zone
  });
}

export async function deleteZone(
  request: Request,
  response: Response
): Promise<void> {
  const rideCount = await prisma.ride.count({
    where: { zoneId: String(request.params.id) }
  });

  if (rideCount > 0) {
    throw new AppError(
      409,
      "This zone cannot be deleted while rides are assigned to it."
    );
  }

  await prisma.zone.delete({ where: { id: String(request.params.id) } });

  response.json({
    success: true,
    message: "Zone deleted successfully."
  });
}