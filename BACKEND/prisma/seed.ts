import bcrypt from "bcryptjs";
import {
  AdminRole,
  PrismaClient,
  RideStatus,
  StaffRole
} from "../generated/prisma/client.js";

const prisma = new PrismaClient();

async function main(): Promise<void> {
  const adminPassword = await bcrypt.hash("Admin123!", 12);
  const staffPassword = await bcrypt.hash("Staff123!", 12);
  const customerPassword = await bcrypt.hash("Customer123!", 12);

  await prisma.admin.upsert({
    where: { email: "admin@funpark.test" },
    update: {},
    create: {
      adminName: "System Administrator",
      email: "admin@funpark.test",
      passwordHash: adminPassword,
      role: AdminRole.SUPER_ADMIN,
      phone: "01710000001"
    }
  });

  await prisma.staff.upsert({
    where: { email: "operator@funpark.test" },
    update: {},
    create: {
      name: "Ride Operator",
      email: "operator@funpark.test",
      phone: "01910000001",
      passwordHash: staffPassword,
      role: StaffRole.RIDE_OPERATOR
    }
  });

  await prisma.customer.upsert({
    where: { email: "customer@funpark.test" },
    update: {},
    create: {
      name: "Demo Customer",
      email: "customer@funpark.test",
      phone: "01810000001",
      passwordHash: customerPassword,
      age: 24
    }
  });

  const adventureZone = await prisma.zone.upsert({
    where: { zoneName: "Adventure Zone" },
    update: {},
    create: {
      zoneName: "Adventure Zone",
      description: "High-thrill attractions",
      mapX: 22,
      mapY: 30
    }
  });

  const familyZone = await prisma.zone.upsert({
    where: { zoneName: "Family Zone" },
    update: {},
    create: {
      zoneName: "Family Zone",
      description: "Family and child-friendly attractions",
      mapX: 60,
      mapY: 28
    }
  });

  await prisma.ride.upsert({
    where: { rideName: "Sky Roller Coaster" },
    update: {},
    create: {
      rideName: "Sky Roller Coaster",
      description: "High-speed steel coaster",
      zoneId: adventureZone.id,
      capacity: 24,
      minimumAge: 12,
      intensityLevel: 5,
      cycleDurationMinutes: 4,
      status: RideStatus.ACTIVE,
      currentQueueCount: 42,
      currentWaitTimeMinutes: 35
    }
  });

  await prisma.ride.upsert({
    where: { rideName: "Magic Carousel" },
    update: {},
    create: {
      rideName: "Magic Carousel",
      description: "Classic family carousel",
      zoneId: familyZone.id,
      capacity: 40,
      minimumAge: 3,
      intensityLevel: 1,
      cycleDurationMinutes: 5,
      status: RideStatus.ACTIVE,
      currentQueueCount: 14,
      currentWaitTimeMinutes: 10
    }
  });

  console.log("Database seed completed.");
  console.log("Admin: admin@funpark.test / Admin123!");
  console.log("Staff: operator@funpark.test / Staff123!");
  console.log("Customer: customer@funpark.test / Customer123!");
}

main()
  .catch((error) => {
    console.error(error);
    process.exitCode = 1;
  })
  .finally(async () => {
    await prisma.$disconnect();
  });