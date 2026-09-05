import { Router } from "express";
import { authRouter } from "./auth.routes.js";
import { emergencyRouter } from "./emergency.routes.js";
import { feedbackRouter } from "./feedback.routes.js";
import { maintenanceRouter } from "./maintenance.routes.js";
import { reportRouter } from "./report.routes.js";
import { rideRouter } from "./ride.routes.js";
import { rideUsageRouter } from "./rideUsage.routes.js";
import { ticketRouter } from "./ticket.routes.js";
import { zoneRouter } from "./zone.routes.js";

export const apiRouter = Router();

apiRouter.use("/auth", authRouter);
apiRouter.use("/zones", zoneRouter);
apiRouter.use("/rides", rideRouter);
apiRouter.use("/tickets", ticketRouter);
apiRouter.use("/ride-usages", rideUsageRouter);
apiRouter.use("/maintenance", maintenanceRouter);
apiRouter.use("/feedback", feedbackRouter);
apiRouter.use("/emergency", emergencyRouter);
apiRouter.use("/reports", reportRouter);