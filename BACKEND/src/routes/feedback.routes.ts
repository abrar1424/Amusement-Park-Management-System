import { Router } from "express";
import {
  createFeedback,
  listFeedback,
  updateFeedbackStatus
} from "../controllers/feedback.controller.js";
import { requireAuth } from "../middleware/auth.middleware.js";
import { allowRoles } from "../middleware/role.middleware.js";
import { asyncHandler } from "../utils/asyncHandler.js";

export const feedbackRouter = Router();

feedbackRouter.post(
  "/",
  requireAuth,
  allowRoles("CUSTOMER"),
  asyncHandler(createFeedback)
);

feedbackRouter.get(
  "/",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(listFeedback)
);

feedbackRouter.patch(
  "/:id/status",
  requireAuth,
  allowRoles("ADMIN"),
  asyncHandler(updateFeedbackStatus)
);