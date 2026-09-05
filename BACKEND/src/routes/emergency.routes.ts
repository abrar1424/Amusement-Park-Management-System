import { Router } from "express";
import {
  createAlert,
  createEmergencyRequest,
  listActiveAlerts,
  listEmergencyRequests,
  resolveAlert,
  updateEmergencyRequest
} from "../controllers/emergency.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const emergencyRouter = Router();

emergencyRouter.get("/alerts", asyncHandler(listActiveAlerts));

emergencyRouter.post(
  "/alerts",
  requireAuth,
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(createAlert)
);

emergencyRouter.patch(
  "/alerts/:id/resolve",
  requireAuth,
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(resolveAlert)
);

emergencyRouter.post(
  "/requests",
  requireAuth,
  allowRoles("CUSTOMER"),
  asyncHandler(createEmergencyRequest)
);

emergencyRouter.get(
  "/requests",
  requireAuth,
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(listEmergencyRequests)
);

emergencyRouter.patch(
  "/requests/:id",
  requireAuth,
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(updateEmergencyRequest)
);