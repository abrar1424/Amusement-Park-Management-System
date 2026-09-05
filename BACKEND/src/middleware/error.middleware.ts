import type {
  NextFunction,
  Request,
  Response
} from "express";
import { ZodError } from "zod";
import { AppError } from "../utils/AppError.js";

export function notFoundHandler(
  request: Request,
  _response: Response,
  next: NextFunction
): void {
  next(
    new AppError(
      404,
      `Route not found: ${request.method} ${request.originalUrl}`
    )
  );
}

export function errorHandler(
  error: unknown,
  _request: Request,
  response: Response,
  _next: NextFunction
): void {
  if (error instanceof ZodError) {
    response.status(400).json({
      success: false,
      message: "Validation failed.",
      errors: error.flatten()
    });
    return;
  }

  if (error instanceof AppError) {
    response.status(error.statusCode).json({
      success: false,
      message: error.message,
      details: error.details
    });
    return;
  }

  console.error(error);

  response.status(500).json({
    success: false,
    message: "Internal server error."
  });
}