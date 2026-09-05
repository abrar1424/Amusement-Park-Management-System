import type { NextFunction, Request, Response } from "express";
import { AppError } from "../utils/AppError.js";
import { verifyAccessToken } from "../utils/jwt.js";

export function requireAuth(
  request: Request,
  _response: Response,
  next: NextFunction
): void {
  const authorization = request.headers.authorization;

  if (!authorization?.startsWith("Bearer ")) {
    throw new AppError(401, "Authentication token is required.");
  }

  const token = authorization.slice("Bearer ".length).trim();

  try {
    request.user = verifyAccessToken(token);
    next();
  } catch {
    throw new AppError(401, "Invalid or expired authentication token.");
  }
}   