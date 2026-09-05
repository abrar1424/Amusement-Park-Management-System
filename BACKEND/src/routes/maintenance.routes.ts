import { Router } from "express";
import {
  createMaintenance,
  listMaintenance,
  updateMaintenanceStatus
} from "../controllers/maintenance.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const maintenanceRouter = Router();

maintenanceRouter.use(
  requireAuth,
  allowRoles("ADMIN", "STAFF")
);

maintenanceRouter.get("/", asyncHandler(listMaintenance));
maintenanceRouter.post("/", asyncHandler(createMaintenance));
maintenanceRouter.patch(
  "/:id/status",
  asyncHandler(updateMaintenanceStatus)
);