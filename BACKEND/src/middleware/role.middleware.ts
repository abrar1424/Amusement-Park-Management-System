import type { NextFunction, Request, Response } from "express";
import type { AuthRole } from "../types/auth.js";
import { AppError } from "../utils/AppError.js";

export function allowRoles(...allowedRoles: AuthRole[]) {
  return (
    request: Request,
    _response: Response,
    next: NextFunction
  ): void => {
    if (!request.user) {
      throw new AppError(401, "Authentication is required.");
    }

    if (!allowedRoles.includes(request.user.role)) {
      throw new AppError(403, "You do not have permission for this action.");
    }

    next();
  };
}