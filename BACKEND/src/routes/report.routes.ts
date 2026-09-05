import { Router } from "express";
import {
  dailyVisitorReport,
  dashboardReport,
  popularRidesReport,
  revenueReport,
  satisfactionReport
} from "../controllers/report.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const reportRouter = Router();

reportRouter.use(
  requireAuth,
  allowRoles("ADMIN")
);

reportRouter.get("/dashboard", asyncHandler(dashboardReport));
reportRouter.get("/revenue", asyncHandler(revenueReport));
reportRouter.get("/popular-rides", asyncHandler(popularRidesReport));
reportRouter.get("/daily-visitors", asyncHandler(dailyVisitorReport));
reportRouter.get("/satisfaction", asyncHandler(satisfactionReport));