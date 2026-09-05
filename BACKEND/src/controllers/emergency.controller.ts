import type { Request, Response } from "express";
import { z } from "zod";
import {
  AlertSeverity,
  EmergencyRequestStatus
} from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const alertSchema = z.object({
  title: z.string().min(2).max(150),
  message: z.string().min(5).max(5000),
  severity: z.nativeEnum(AlertSeverity).default(AlertSeverity.INFO),
  zoneId: z.string().uuid().optional()
});

export async function listActiveAlerts(
  _request: Request,
  response: Response
): Promise<void> {
  const alerts = await prisma.emergencyAlert.findMany({
    where: { isActive: true },
    include: { zone: true },
    orderBy: { createdAt: "desc" }
  });

  response.json({ success: true, data: alerts });
}

export async function createAlert(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user) {
    throw new AppError(401, "Authentication is required.");
  }

  const data = alertSchema.parse(request.body);

  const alert = await prisma.emergencyAlert.create({
    data: {
      ...data,
      createdByAdminId:
        request.user.role === "ADMIN" ? request.user.id : null,
      createdByStaffId:
        request.user.role === "STAFF" ? request.user.id : null
    }
  });

  response.status(201).json({
    success: true,
    message: "Emergency alert created successfully.",
    data: alert
  });
}

export async function resolveAlert(
  request: Request,
  response: Response
): Promise<void> {
  const alert = await prisma.emergencyAlert.update({
    where: { id: String(request.params.id) },
    data: {
      isActive: false,
      resolvedAt: new Date()
    }
  });

  response.json({
    success: true,
    message: "Emergency alert resolved successfully.",
    data: alert
  });
}

const requestSchema = z.object({
  message: z.string().min(5).max(5000),
  locationNote: z.string().max(255).optional(),
  zoneId: z.string().uuid().optional()
});

export async function createEmergencyRequest(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user || request.user.role !== "CUSTOMER") {
    throw new AppError(403, "Only customers can request emergency help.");
  }

  const data = requestSchema.parse(request.body);

  const emergencyRequest = await prisma.emergencyRequest.create({
    data: {
      ...data,
      customerId: request.user.id
    }
  });

  response.status(201).json({
    success: true,
    message: "Emergency request submitted successfully.",
    data: emergencyRequest
  });
}

export async function listEmergencyRequests(
  _request: Request,
  response: Response
): Promise<void> {
  const requests = await prisma.emergencyRequest.findMany({
    include: {
      customer: { select: { id: true, name: true, phone: true } },
      zone: true,
      assignedStaff: {
        select: { id: true, name: true, phone: true }
      }
    },
    orderBy: { createdAt: "desc" }
  });

  response.json({ success: true, data: requests });
}

export async function updateEmergencyRequest(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user) {
    throw new AppError(401, "Authentication is required.");
  }

  const data = z.object({
    status: z.nativeEnum(EmergencyRequestStatus),
    assignedStaffId: z.string().uuid().nullable().optional()
  }).parse(request.body);

  const statusData =
    data.status === EmergencyRequestStatus.RESOLVED
      ? { resolvedAt: new Date() }
      : { resolvedAt: null };

  const emergencyRequest = await prisma.emergencyRequest.update({
    where: { id: String(request.params.id) },
    data: {
      status: data.status,
      assignedStaffId:
        data.assignedStaffId ??
        (request.user.role === "STAFF" ? request.user.id : undefined),
      ...statusData
    }
  });

  response.json({
    success: true,
    message: "Emergency request updated successfully.",
    data: emergencyRequest
  });
}