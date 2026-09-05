import type { Request, Response } from "express";
import { z } from "zod";
import {
  FeedbackStatus,
  FeedbackType
} from "../../generated/prisma/client.js";
import { prisma } from "../lib/prisma.js";
import { AppError } from "../utils/AppError.js";

const feedbackSchema = z.object({
  rideId: z.string().uuid().optional(),
  type: z.nativeEnum(FeedbackType).default(FeedbackType.FEEDBACK),
  rating: z.coerce.number().int().min(1).max(5),
  comments: z.string().max(5000).optional()
});

export async function createFeedback(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user || request.user.role !== "CUSTOMER") {
    throw new AppError(403, "Only customers can submit feedback.");
  }

  const data = feedbackSchema.parse(request.body);

  if (data.rideId) {
    const ride = await prisma.ride.findUnique({
      where: { id: data.rideId }
    });

    if (!ride) {
      throw new AppError(404, "Ride not found.");
    }
  }

  const feedback = await prisma.feedback.create({
    data: {
      ...data,
      customerId: request.user.id
    }
  });

  response.status(201).json({
    success: true,
    message: "Feedback submitted successfully.",
    data: feedback
  });
}

export async function listFeedback(
  _request: Request,
  response: Response
): Promise<void> {
  const feedback = await prisma.feedback.findMany({
    include: {
      customer: { select: { id: true, name: true, email: true } },
      ride: { select: { id: true, rideName: true } }
    },
    orderBy: { submittedAt: "desc" }
  });

  response.json({ success: true, data: feedback });
}

export async function updateFeedbackStatus(
  request: Request,
  response: Response
): Promise<void> {
  const data = z.object({
    status: z.nativeEnum(FeedbackStatus)
  }).parse(request.body);

  const feedback = await prisma.feedback.update({
    where: { id: String(request.params.id) },
    data
  });

  response.json({
    success: true,
    message: "Feedback status updated successfully.",
    data: feedback
  });
}