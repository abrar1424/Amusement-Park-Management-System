import { Router } from "express";
import {
  createRide,
  deleteRide,
  getRide,
  listRides,
  updateRide,
  updateRideOperation
} from "../controllers/ride.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const rideRouter = Router();

rideRouter.get("/", asyncHandler(listRides));
rideRouter.get("/:id", asyncHandler(getRide));
rideRouter.post(
  "/",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(createRide)
);
rideRouter.patch(
  "/:id",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(updateRide)
);
rideRouter.patch(
  "/:id/operation",
  requireAuth,
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(updateRideOperation)
);
rideRouter.delete(
  "/:id",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(deleteRide)
);