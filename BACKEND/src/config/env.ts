import "dotenv/config";
import { z } from "zod";

const environmentSchema = z.object({
  DATABASE_URL: z.string().min(1),
  PORT: z.coerce.number().int().positive().default(5000),
  NODE_ENV: z
    .enum(["development", "test", "production"])
    .default("development"),
  CLIENT_URL: z.string().url().default("http://localhost:5173"),
  JWT_SECRET: z.string().min(20),
  JWT_EXPIRES_IN: z.string().default("7d")
});

const result = environmentSchema.safeParse(process.env);

if (!result.success) {
  console.error("Invalid environment variables:");
  console.error(result.error.flatten().fieldErrors);
  process.exit(1);
}

export const env = result.data;

Backend/src/controllers/auth.controller.ts

import bcrypt from "bcryptjs";
import type { Request, Response } from "express";
import { z } from "zod";
import { prisma } from "../lib/prisma.js";
import type { AuthUser } from "../types/auth.js";
import { AppError } from "../utils/AppError.js";
import { createAccessToken } from "../utils/jwt.js";

const registrationSchema = z.object({
  name: z.string().min(2).max(100),
  email: z.string().email().max(255),
  phone: z.string().min(7).max(20),
  password: z.string().min(6).max(100),
  age: z.coerce.number().int().min(1).max(120)
});

const loginSchema = z.object({
  email: z.string().email(),
  password: z.string().min(1)
});

export async function registerCustomer(
  request: Request,
  response: Response
): Promise<void> {
  const data = registrationSchema.parse(request.body);

  const duplicate = await prisma.customer.findFirst({
    where: {
      OR: [
        { email: data.email.toLowerCase() },
        { phone: data.phone }
      ]
    }
  });

  if (duplicate) {
    throw new AppError(409, "Email or phone is already registered.");
  }

  const customer = await prisma.customer.create({
    data: {
      name: data.name,
      email: data.email.toLowerCase(),
      phone: data.phone,
      passwordHash: await bcrypt.hash(data.password, 12),
      age: data.age
    },
    select: {
      id: true,
      name: true,
      email: true,
      phone: true,
      age: true,
      createdAt: true
    }
  });

  const authUser: AuthUser = {
    id: customer.id,
    email: customer.email,
    role: "CUSTOMER"
  };

  response.status(201).json({
    success: true,
    message: "Customer registered successfully.",
    data: {
      user: customer,
      token: createAccessToken(authUser)
    }
  });
}

export async function login(
  request: Request,
  response: Response
): Promise<void> {
  const data = loginSchema.parse(request.body);
  const email = data.email.toLowerCase();

  const admin = await prisma.admin.findUnique({ where: { email } });

  if (admin && await bcrypt.compare(data.password, admin.passwordHash)) {
    await prisma.admin.update({
      where: { id: admin.id },
      data: { lastLogin: new Date() }
    });

    const user: AuthUser = {
      id: admin.id,
      email: admin.email,
      role: "ADMIN"
    };

    response.json({
      success: true,
      data: {
        token: createAccessToken(user),
        user: {
          id: admin.id,
          name: admin.adminName,
          email: admin.email,
          role: "ADMIN"
        }
      }
    });
    return;
  }

  const staff = await prisma.staff.findUnique({ where: { email } });

  if (
    staff &&
    staff.isActive &&
    await bcrypt.compare(data.password, staff.passwordHash)
  ) {
    const user: AuthUser = {
      id: staff.id,
      email: staff.email,
      role: "STAFF"
    };

    response.json({
      success: true,
      data: {
        token: createAccessToken(user),
        user: {
          id: staff.id,
          name: staff.name,
          email: staff.email,
          role: "STAFF",
          staffRole: staff.role
        }
      }
    });
    return;
  }

  const customer = await prisma.customer.findUnique({ where: { email } });

  if (
    customer &&
    customer.isActive &&
    await bcrypt.compare(data.password, customer.passwordHash)
  ) {
    const user: AuthUser = {
      id: customer.id,
      email: customer.email,
      role: "CUSTOMER"
    };

    response.json({
      success: true,
      data: {
        token: createAccessToken(user),
        user: {
          id: customer.id,
          name: customer.name,
          email: customer.email,
          role: "CUSTOMER"
        }
      }
    });
    return;
  }

  throw new AppError(401, "Invalid email or password.");
}

export async function getCurrentUser(
  request: Request,
  response: Response
): Promise<void> {
  if (!request.user) {
    throw new AppError(401, "Authentication is required.");
  }

  let profile: unknown;

  if (request.user.role === "ADMIN") {
    profile = await prisma.admin.findUnique({
      where: { id: request.user.id },
      select: {
        id: true,
        adminName: true,
        email: true,
        role: true,
        phone: true,
        lastLogin: true
      }
    });
  } else if (request.user.role === "STAFF") {
    profile = await prisma.staff.findUnique({
      where: { id: request.user.id },
      select: {
        id: true,
        name: true,
        email: true,
        phone: true,
        role: true,
        isActive: true
      }
    });
  } else {
    profile = await prisma.customer.findUnique({
      where: { id: request.user.id },
      select: {
        id: true,
        name: true,
        email: true,
        phone: true,
        age: true,
        isActive: true
      }
    });
  }

  if (!profile) {
    throw new AppError(404, "User account was not found.");
  }

  response.json({ success: true, data: profile });
}