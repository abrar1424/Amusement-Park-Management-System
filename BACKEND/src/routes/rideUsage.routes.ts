import { Router } from "express";
import {
  listRideUsage,
  recordRideUsage
} from "../controllers/rideUsage.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const rideUsageRouter = Router();

rideUsageRouter.use(
  requireAuth,
  allowRoles("ADMIN", "STAFF")
);

rideUsageRouter.get("/", asyncHandler(listRideUsage));
rideUsageRouter.post("/", asyncHandler(recordRideUsage));