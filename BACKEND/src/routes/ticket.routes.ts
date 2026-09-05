import { Router } from "express";
import {
  bookTicket,
  getTicketQrCode,
  listMyTickets,
  payTicket,
  recordParkExit,
  verifyParkEntry
} from "../controllers/ticket.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const ticketRouter = Router();

ticketRouter.use(requireAuth);

ticketRouter.post(
  "/",
  allowRoles("CUSTOMER"),
  asyncHandler(bookTicket)
);
ticketRouter.get(
  "/my",
  allowRoles("CUSTOMER"),
  asyncHandler(listMyTickets)
);
ticketRouter.post(
  "/:id/pay",
  allowRoles("CUSTOMER"),
  asyncHandler(payTicket)
);
ticketRouter.get("/:id/qr", asyncHandler(getTicketQrCode));
ticketRouter.post(
  "/verify-entry",
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(verifyParkEntry)
);
ticketRouter.post(
  "/record-exit",
  allowRoles("ADMIN", "STAFF"),
  asyncHandler(recordParkExit)
);