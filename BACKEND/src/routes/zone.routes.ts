import { Router } from "express";
import {
  createZone,
  deleteZone,
  listZones,
  updateZone
} from "../controllers/zone.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const zoneRouter = Router();

zoneRouter.get("/", asyncHandler(listZones));
zoneRouter.post(
  "/",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(createZone)
);
zoneRouter.patch(
  "/:id",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(updateZone)
);
zoneRouter.delete(
  "/:id",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(deleteZone)
);