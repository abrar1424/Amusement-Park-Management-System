CREATE DATABASE amusement_park_db;
USE amusement_park_db;-- ============================================================
-- AMUSEMENT PARK MANAGEMENT SYSTEM
-- COMPLETE SINGLE POSTGRESQL SCRIPT
-- Tables + Relationships + Indexes + 30 Sample Rows + 20 Queries
-- ============================================================

-- ============================================================
-- 0. REMOVE OLD TABLES AND TYPES
-- This section allows the script to be run again.
-- ============================================================

DROP TABLE IF EXISTS emergency_requests CASCADE;
DROP TABLE IF EXISTS emergency_alerts CASCADE;
DROP TABLE IF EXISTS feedback CASCADE;
DROP TABLE IF EXISTS maintenance CASCADE;
DROP TABLE IF EXISTS ride_queue_snapshots CASCADE;
DROP TABLE IF EXISTS ride_usage CASCADE;
DROP TABLE IF EXISTS park_entries CASCADE;
DROP TABLE IF EXISTS payment_transactions CASCADE;
DROP TABLE IF EXISTS tickets CASCADE;
DROP TABLE IF EXISTS rides CASCADE;
DROP TABLE IF EXISTS staff CASCADE;
DROP TABLE IF EXISTS zones CASCADE;
DROP TABLE IF EXISTS customers CASCADE;
DROP TABLE IF EXISTS admins CASCADE;

DROP TYPE IF EXISTS "EmergencyRequestStatus" CASCADE;
DROP TYPE IF EXISTS "AlertSeverity" CASCADE;
DROP TYPE IF EXISTS "EntryStatus" CASCADE;
DROP TYPE IF EXISTS "FeedbackType" CASCADE;
DROP TYPE IF EXISTS "FeedbackStatus" CASCADE;
DROP TYPE IF EXISTS "MaintenanceStatus" CASCADE;
DROP TYPE IF EXISTS "StaffRole" CASCADE;
DROP TYPE IF EXISTS "RideStatus" CASCADE;
DROP TYPE IF EXISTS "PaymentMethod" CASCADE;
DROP TYPE IF EXISTS "PaymentStatus" CASCADE;
DROP TYPE IF EXISTS "TicketType" CASCADE;
DROP TYPE IF EXISTS "AdminRole" CASCADE;

-- ============================================================
-- 1. CREATE ENUM TYPES
-- ============================================================

CREATE TYPE "AdminRole" AS ENUM (
    'SUPER_ADMIN',
    'MANAGER',
    'ANALYST'
);

CREATE TYPE "TicketType" AS ENUM (
    'CHILD',
    'ADULT',
    'SENIOR',
    'FAMILY',
    'VIP'
);

CREATE TYPE "PaymentStatus" AS ENUM (
    'PENDING',
    'PAID',
    'FAILED',
    'REFUNDED'
);

CREATE TYPE "PaymentMethod" AS ENUM (
    'CASH',
    'CARD',
    'MOBILE_BANKING',
    'DEMO_GATEWAY'
);

CREATE TYPE "RideStatus" AS ENUM (
    'ACTIVE',
    'CLOSED',
    'UNDER_MAINTENANCE'
);

CREATE TYPE "StaffRole" AS ENUM (
    'RIDE_OPERATOR',
    'MAINTENANCE_TECHNICIAN',
    'SECURITY',
    'TICKET_OFFICER',
    'MANAGER'
);

CREATE TYPE "MaintenanceStatus" AS ENUM (
    'REQUESTED',
    'ASSIGNED',
    'IN_PROGRESS',
    'COMPLETED',
    'CANCELLED'
);

CREATE TYPE "FeedbackStatus" AS ENUM (
    'PENDING',
    'REVIEWED',
    'RESOLVED'
);

CREATE TYPE "FeedbackType" AS ENUM (
    'FEEDBACK',
    'COMPLAINT',
    'SAFETY_REPORT'
);

CREATE TYPE "EntryStatus" AS ENUM (
    'ENTERED',
    'EXITED',
    'DENIED'
);

CREATE TYPE "AlertSeverity" AS ENUM (
    'INFO',
    'WARNING',
    'CRITICAL'
);

CREATE TYPE "EmergencyRequestStatus" AS ENUM (
    'OPEN',
    'ACKNOWLEDGED',
    'RESOLVED'
);

-- ============================================================
-- 2. CREATE ADMINS TABLE
-- ============================================================

CREATE TABLE admins (
    admin_id SERIAL PRIMARY KEY,

    admin_name VARCHAR(100) NOT NULL,

    email VARCHAR(255) NOT NULL UNIQUE,

    password_hash VARCHAR(255) NOT NULL,

    role "AdminRole" NOT NULL DEFAULT 'MANAGER',

    phone VARCHAR(20),

    last_login TIMESTAMP(3),

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    updated_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================
-- 3. CREATE CUSTOMERS TABLE
-- ============================================================

CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,

    name VARCHAR(100) NOT NULL,

    phone VARCHAR(20) NOT NULL UNIQUE,

    email VARCHAR(255) NOT NULL UNIQUE,

    password_hash VARCHAR(255) NOT NULL,

    age INTEGER NOT NULL
        CHECK (age BETWEEN 1 AND 120),

    is_active BOOLEAN
        NOT NULL DEFAULT TRUE,

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    updated_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================
-- 4. CREATE ZONES TABLE
-- ============================================================

CREATE TABLE zones (
    zone_id SERIAL PRIMARY KEY,

    zone_name VARCHAR(100)
        NOT NULL UNIQUE,

    description TEXT,

    map_x INTEGER
        CHECK (map_x BETWEEN 0 AND 100),

    map_y INTEGER
        CHECK (map_y BETWEEN 0 AND 100)
);

-- ============================================================
-- 5. CREATE STAFF TABLE
-- ============================================================

CREATE TABLE staff (
    staff_id SERIAL PRIMARY KEY,

    name VARCHAR(100) NOT NULL,

    role "StaffRole" NOT NULL,

    phone VARCHAR(20)
        NOT NULL UNIQUE,

    email VARCHAR(255)
        NOT NULL UNIQUE,

    password_hash VARCHAR(255)
        NOT NULL,

    is_active BOOLEAN
        NOT NULL DEFAULT TRUE,

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    updated_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- ============================================================
-- 6. CREATE RIDES TABLE
-- ============================================================

CREATE TABLE rides (
    ride_id SERIAL PRIMARY KEY,

    ride_name VARCHAR(120)
        NOT NULL UNIQUE,

    description TEXT,

    zone_id INTEGER NOT NULL,

    capacity INTEGER NOT NULL
        CHECK (capacity > 0),

    minimum_age INTEGER
        NOT NULL DEFAULT 5
        CHECK (minimum_age BETWEEN 1 AND 100),

    intensity_level INTEGER
        NOT NULL DEFAULT 1
        CHECK (intensity_level BETWEEN 1 AND 5),

    cycle_duration_minutes INTEGER
        NOT NULL DEFAULT 5
        CHECK (cycle_duration_minutes > 0),

    status "RideStatus"
        NOT NULL DEFAULT 'ACTIVE',

    current_queue_count INTEGER
        NOT NULL DEFAULT 0
        CHECK (current_queue_count >= 0),

    current_wait_time_minutes INTEGER
        NOT NULL DEFAULT 0
        CHECK (current_wait_time_minutes >= 0),

    last_status_updated_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT rides_zone_id_fkey
        FOREIGN KEY (zone_id)
        REFERENCES zones(zone_id)
        ON DELETE RESTRICT
        ON UPDATE CASCADE
);

-- ============================================================
-- 7. CREATE TICKETS TABLE
-- ============================================================

CREATE TABLE tickets (
    ticket_id SERIAL PRIMARY KEY,

    customer_id INTEGER NOT NULL,

    visit_date DATE NOT NULL,

    ticket_type "TicketType" NOT NULL,

    ticket_price DECIMAL(10,2)
        NOT NULL
        CHECK (ticket_price >= 0),

    payment_status "PaymentStatus"
        NOT NULL DEFAULT 'PENDING',

    payment_date TIMESTAMP(3),

    qr_code VARCHAR(255)
        NOT NULL UNIQUE,

    booked_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT tickets_customer_id_fkey
        FOREIGN KEY (customer_id)
        REFERENCES customers(customer_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- ============================================================
-- 8. CREATE PAYMENT TRANSACTIONS TABLE
-- ============================================================

CREATE TABLE payment_transactions (
    payment_id SERIAL PRIMARY KEY,

    ticket_id INTEGER NOT NULL,

    amount DECIMAL(10,2)
        NOT NULL
        CHECK (amount >= 0),

    payment_method "PaymentMethod"
        NOT NULL,

    transaction_reference VARCHAR(100)
        NOT NULL UNIQUE,

    status "PaymentStatus"
        NOT NULL DEFAULT 'PENDING',

    paid_at TIMESTAMP(3),

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT payment_transactions_ticket_id_fkey
        FOREIGN KEY (ticket_id)
        REFERENCES tickets(ticket_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- ============================================================
-- 9. CREATE PARK ENTRIES TABLE
-- ============================================================

CREATE TABLE park_entries (
    entry_id SERIAL PRIMARY KEY,

    ticket_id INTEGER NOT NULL,

    gate_name VARCHAR(100) NOT NULL,

    entry_time TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    exit_time TIMESTAMP(3),

    status "EntryStatus"
        NOT NULL DEFAULT 'ENTERED',

    CONSTRAINT valid_exit_time
        CHECK (
            exit_time IS NULL
            OR exit_time >= entry_time
        ),

    CONSTRAINT park_entries_ticket_id_fkey
        FOREIGN KEY (ticket_id)
        REFERENCES tickets(ticket_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- ============================================================
-- 10. CREATE RIDE USAGE TABLE
-- ============================================================

CREATE TABLE ride_usage (
    usage_id SERIAL PRIMARY KEY,

    customer_id INTEGER NOT NULL,

    ride_id INTEGER NOT NULL,

    ticket_id INTEGER,

    entry_time TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT ride_usage_customer_id_fkey
        FOREIGN KEY (customer_id)
        REFERENCES customers(customer_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT ride_usage_ride_id_fkey
        FOREIGN KEY (ride_id)
        REFERENCES rides(ride_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT ride_usage_ticket_id_fkey
        FOREIGN KEY (ticket_id)
        REFERENCES tickets(ticket_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

-- ============================================================
-- 11. CREATE RIDE QUEUE SNAPSHOTS TABLE
-- ============================================================

CREATE TABLE ride_queue_snapshots (
    snapshot_id SERIAL PRIMARY KEY,

    ride_id INTEGER NOT NULL,

    waiting_count INTEGER NOT NULL
        CHECK (waiting_count >= 0),

    estimated_wait_minutes INTEGER NOT NULL
        CHECK (estimated_wait_minutes >= 0),

    recorded_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT ride_queue_snapshots_ride_id_fkey
        FOREIGN KEY (ride_id)
        REFERENCES rides(ride_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
);

-- ============================================================
-- 12. CREATE MAINTENANCE TABLE
-- ============================================================

CREATE TABLE maintenance (
    maintenance_id SERIAL PRIMARY KEY,

    ride_id INTEGER NOT NULL,

    staff_id INTEGER,

    issue_description TEXT NOT NULL,

    maintenance_date DATE NOT NULL,

    status "MaintenanceStatus"
        NOT NULL DEFAULT 'REQUESTED',

    completed_at TIMESTAMP(3),

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT maintenance_ride_id_fkey
        FOREIGN KEY (ride_id)
        REFERENCES rides(ride_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT maintenance_staff_id_fkey
        FOREIGN KEY (staff_id)
        REFERENCES staff(staff_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

-- ============================================================
-- 13. CREATE FEEDBACK TABLE
-- ============================================================

CREATE TABLE feedback (
    feedback_id SERIAL PRIMARY KEY,

    customer_id INTEGER NOT NULL,

    ride_id INTEGER,

    type "FeedbackType"
        NOT NULL DEFAULT 'FEEDBACK',

    rating INTEGER NOT NULL
        CHECK (rating BETWEEN 1 AND 5),

    comments TEXT,

    status "FeedbackStatus"
        NOT NULL DEFAULT 'PENDING',

    submitted_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT feedback_customer_id_fkey
        FOREIGN KEY (customer_id)
        REFERENCES customers(customer_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT feedback_ride_id_fkey
        FOREIGN KEY (ride_id)
        REFERENCES rides(ride_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

-- ============================================================
-- 14. CREATE EMERGENCY ALERTS TABLE
-- ============================================================

CREATE TABLE emergency_alerts (
    alert_id SERIAL PRIMARY KEY,

    title VARCHAR(150) NOT NULL,

    message TEXT NOT NULL,

    severity "AlertSeverity"
        NOT NULL DEFAULT 'INFO',

    zone_id INTEGER,

    created_by_admin_id INTEGER,

    created_by_staff_id INTEGER,

    is_active BOOLEAN
        NOT NULL DEFAULT TRUE,

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    resolved_at TIMESTAMP(3),

    CONSTRAINT emergency_alerts_zone_id_fkey
        FOREIGN KEY (zone_id)
        REFERENCES zones(zone_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE,

    CONSTRAINT emergency_alerts_admin_id_fkey
        FOREIGN KEY (created_by_admin_id)
        REFERENCES admins(admin_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE,

    CONSTRAINT emergency_alerts_staff_id_fkey
        FOREIGN KEY (created_by_staff_id)
        REFERENCES staff(staff_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

-- ============================================================
-- 15. CREATE EMERGENCY REQUESTS TABLE
-- ============================================================

CREATE TABLE emergency_requests (
    request_id SERIAL PRIMARY KEY,

    customer_id INTEGER NOT NULL,

    zone_id INTEGER,

    assigned_staff_id INTEGER,

    message TEXT NOT NULL,

    location_note VARCHAR(255),

    status "EmergencyRequestStatus"
        NOT NULL DEFAULT 'OPEN',

    created_at TIMESTAMP(3)
        NOT NULL DEFAULT CURRENT_TIMESTAMP,

    resolved_at TIMESTAMP(3),

    CONSTRAINT emergency_requests_customer_id_fkey
        FOREIGN KEY (customer_id)
        REFERENCES customers(customer_id)
        ON DELETE CASCADE
        ON UPDATE CASCADE,

    CONSTRAINT emergency_requests_zone_id_fkey
        FOREIGN KEY (zone_id)
        REFERENCES zones(zone_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE,

    CONSTRAINT emergency_requests_staff_id_fkey
        FOREIGN KEY (assigned_staff_id)
        REFERENCES staff(staff_id)
        ON DELETE SET NULL
        ON UPDATE CASCADE
);

-- ============================================================
-- 16. CREATE INDEXES
-- ============================================================

CREATE INDEX tickets_customer_id_idx
    ON tickets(customer_id);

CREATE INDEX tickets_visit_date_idx
    ON tickets(visit_date);

CREATE INDEX tickets_payment_status_idx
    ON tickets(payment_status);

CREATE INDEX payment_transactions_ticket_id_idx
    ON payment_transactions(ticket_id);

CREATE INDEX payment_transactions_status_idx
    ON payment_transactions(status);

CREATE INDEX rides_zone_id_idx
    ON rides(zone_id);

CREATE INDEX rides_status_idx
    ON rides(status);

CREATE INDEX park_entries_ticket_id_idx
    ON park_entries(ticket_id);

CREATE INDEX park_entries_entry_time_idx
    ON park_entries(entry_time);

CREATE INDEX ride_usage_customer_id_idx
    ON ride_usage(customer_id);

CREATE INDEX ride_usage_ride_id_idx
    ON ride_usage(ride_id);

CREATE INDEX ride_usage_entry_time_idx
    ON ride_usage(entry_time);

CREATE INDEX ride_queue_snapshots_ride_id_recorded_at_idx
    ON ride_queue_snapshots(ride_id, recorded_at);

CREATE INDEX maintenance_ride_id_idx
    ON maintenance(ride_id);

CREATE INDEX maintenance_staff_id_idx
    ON maintenance(staff_id);

CREATE INDEX maintenance_status_idx
    ON maintenance(status);

CREATE INDEX feedback_customer_id_idx
    ON feedback(customer_id);

CREATE INDEX feedback_ride_id_idx
    ON feedback(ride_id);

CREATE INDEX feedback_rating_idx
    ON feedback(rating);

CREATE INDEX emergency_alerts_is_active_created_at_idx
    ON emergency_alerts(is_active, created_at);

CREATE INDEX emergency_requests_status_created_at_idx
    ON emergency_requests(status, created_at);

-- ============================================================
-- 17. INSERT EXACTLY 30 SAMPLE ROWS
-- ============================================================

-- Row 1: Admin

INSERT INTO admins (
    admin_id,
    admin_name,
    email,
    password_hash,
    role,
    phone
)
VALUES (
    1,
    'Farhan Ahmed',
    'admin@funpark.test',
    '$2b$12$qTMcFPZvBKb.ENnOteusp./COpU374Ap8wbvvswspplo7FHpgt7Tm',
    'SUPER_ADMIN',
    '01710000001'
);

-- Rows 2-3: Zones

INSERT INTO zones (
    zone_id,
    zone_name,
    description,
    map_x,
    map_y
)
VALUES
(
    1,
    'Adventure Zone',
    'High-thrill attractions',
    22,
    30
),
(
    2,
    'Family Zone',
    'Family and child-friendly attractions',
    60,
    28
);

-- Rows 4-7: Customers

INSERT INTO customers (
    customer_id,
    name,
    phone,
    email,
    password_hash,
    age
)
VALUES
(
    1,
    'Arafat Hossain',
    '01810000001',
    'customer@funpark.test',
    '$2b$12$cOsBAL0vuxfOQDKpJsU1guZO7Zx54z6.hyHeKeNlbApf6juPWAiym',
    24
),
(
    2,
    'Nabila Rahman',
    '01810000002',
    'nabila@example.com',
    '$2b$12$cOsBAL0vuxfOQDKpJsU1guZO7Zx54z6.hyHeKeNlbApf6juPWAiym',
    27
),
(
    3,
    'Tanvir Hasan',
    '01810000003',
    'tanvir@example.com',
    '$2b$12$cOsBAL0vuxfOQDKpJsU1guZO7Zx54z6.hyHeKeNlbApf6juPWAiym',
    14
),
(
    4,
    'Sabila Islam',
    '01810000004',
    'sabila@example.com',
    '$2b$12$cOsBAL0vuxfOQDKpJsU1guZO7Zx54z6.hyHeKeNlbApf6juPWAiym',
    32
);

-- Rows 8-9: Staff

INSERT INTO staff (
    staff_id,
    name,
    role,
    phone,
    email,
    password_hash
)
VALUES
(
    1,
    'Rashed Karim',
    'RIDE_OPERATOR',
    '01910000001',
    'operator@funpark.test',
    '$2b$12$WcSwDzA/yOJ5.7IW0u65nu93Sl/I/toHMyT1wj8JMcv4zkAVtiVRW'
),
(
    2,
    'Mitu Akter',
    'MAINTENANCE_TECHNICIAN',
    '01910000002',
    'maintenance@funpark.test',
    '$2b$12$WcSwDzA/yOJ5.7IW0u65nu93Sl/I/toHMyT1wj8JMcv4zkAVtiVRW'
);

-- Rows 10-13: Rides

INSERT INTO rides (
    ride_id,
    ride_name,
    description,
    zone_id,
    capacity,
    minimum_age,
    intensity_level,
    cycle_duration_minutes,
    status,
    current_queue_count,
    current_wait_time_minutes
)
VALUES
(
    1,
    'Sky Roller Coaster',
    'High-speed steel coaster',
    1,
    24,
    12,
    5,
    4,
    'ACTIVE',
    42,
    35
),
(
    2,
    'Pirate Ship',
    'Swinging pirate ship',
    1,
    30,
    10,
    4,
    6,
    'ACTIVE',
    26,
    20
),
(
    3,
    'Magic Carousel',
    'Classic family carousel',
    2,
    40,
    3,
    1,
    5,
    'ACTIVE',
    14,
    10
),
(
    4,
    'Mini Ferris Wheel',
    'Panoramic family wheel',
    2,
    20,
    5,
    2,
    8,
    'UNDER_MAINTENANCE',
    0,
    0
);

-- Rows 14-18: Tickets

INSERT INTO tickets (
    ticket_id,
    customer_id,
    visit_date,
    ticket_type,
    ticket_price,
    payment_status,
    payment_date,
    qr_code,
    booked_at
)
VALUES
(
    1,
    1,
    '2026-07-15',
    'ADULT',
    800.00,
    'PAID',
    '2026-07-14 19:00:00',
    'QR-20260715-A001',
    '2026-07-14 18:55:00'
),
(
    2,
    2,
    '2026-07-15',
    'VIP',
    1500.00,
    'PAID',
    '2026-07-14 20:10:00',
    'QR-20260715-V002',
    '2026-07-14 20:05:00'
),
(
    3,
    3,
    '2026-07-16',
    'CHILD',
    500.00,
    'PAID',
    '2026-07-15 17:30:00',
    'QR-20260716-C003',
    '2026-07-15 17:25:00'
),
(
    4,
    4,
    '2026-07-16',
    'ADULT',
    800.00,
    'PENDING',
    NULL,
    'QR-20260716-A004',
    '2026-07-15 21:00:00'
),
(
    5,
    4,
    '2026-07-17',
    'FAMILY',
    2200.00,
    'PAID',
    '2026-07-16 22:15:00',
    'QR-20260717-F005',
    '2026-07-16 22:10:00'
);

-- Rows 19-22: Payments

INSERT INTO payment_transactions (
    payment_id,
    ticket_id,
    amount,
    payment_method,
    transaction_reference,
    status,
    paid_at
)
VALUES
(
    1,
    1,
    800.00,
    'DEMO_GATEWAY',
    'TXN-001',
    'PAID',
    '2026-07-14 19:00:00'
),
(
    2,
    2,
    1500.00,
    'CARD',
    'TXN-002',
    'PAID',
    '2026-07-14 20:10:00'
),
(
    3,
    3,
    500.00,
    'MOBILE_BANKING',
    'TXN-003',
    'PAID',
    '2026-07-15 17:30:00'
),
(
    4,
    5,
    2200.00,
    'DEMO_GATEWAY',
    'TXN-004',
    'PAID',
    '2026-07-16 22:15:00'
);

-- Rows 23-25: Park Entries

INSERT INTO park_entries (
    entry_id,
    ticket_id,
    gate_name,
    entry_time,
    exit_time,
    status
)
VALUES
(
    1,
    1,
    'Main Gate',
    '2026-07-15 09:05:00',
    '2026-07-15 17:45:00',
    'EXITED'
),
(
    2,
    2,
    'VIP Gate',
    '2026-07-15 09:20:00',
    '2026-07-15 18:10:00',
    'EXITED'
),
(
    3,
    3,
    'Main Gate',
    '2026-07-16 10:00:00',
    NULL,
    'ENTERED'
);

-- Rows 26-28: Ride Usage

INSERT INTO ride_usage (
    usage_id,
    customer_id,
    ride_id,
    ticket_id,
    entry_time
)
VALUES
(
    1,
    1,
    1,
    1,
    '2026-07-15 10:15:00'
),
(
    2,
    1,
    2,
    1,
    '2026-07-15 11:20:00'
),
(
    3,
    2,
    1,
    2,
    '2026-07-15 10:30:00'
);

-- Row 29: Maintenance

INSERT INTO maintenance (
    maintenance_id,
    ride_id,
    staff_id,
    issue_description,
    maintenance_date,
    status
)
VALUES (
    1,
    4,
    2,
    'Abnormal vibration detected in wheel motor',
    '2026-07-17',
    'IN_PROGRESS'
);

-- Row 30: Feedback

INSERT INTO feedback (
    feedback_id,
    customer_id,
    ride_id,
    type,
    rating,
    comments,
    status,
    submitted_at
)
VALUES (
    1,
    1,
    1,
    'FEEDBACK',
    5,
    'Exciting ride and the queue moved quickly.',
    'REVIEWED',
    '2026-07-15 18:00:00'
);

-- ============================================================
-- 18. ALIGN SERIAL SEQUENCES AFTER EXPLICIT IDS
-- ============================================================

SELECT setval(
    pg_get_serial_sequence('admins', 'admin_id'),
    COALESCE(MAX(admin_id), 1),
    TRUE
)
FROM admins;

SELECT setval(
    pg_get_serial_sequence('zones', 'zone_id'),
    COALESCE(MAX(zone_id), 1),
    TRUE
)
FROM zones;

SELECT setval(
    pg_get_serial_sequence('customers', 'customer_id'),
    COALESCE(MAX(customer_id), 1),
    TRUE
)
FROM customers;

SELECT setval(
    pg_get_serial_sequence('staff', 'staff_id'),
    COALESCE(MAX(staff_id), 1),
    TRUE
)
FROM staff;

SELECT setval(
    pg_get_serial_sequence('rides', 'ride_id'),
    COALESCE(MAX(ride_id), 1),
    TRUE
)
FROM rides;

SELECT setval(
    pg_get_serial_sequence('tickets', 'ticket_id'),
    COALESCE(MAX(ticket_id), 1),
    TRUE
)
FROM tickets;

SELECT setval(
    pg_get_serial_sequence(
        'payment_transactions',
        'payment_id'
    ),
    COALESCE(MAX(payment_id), 1),
    TRUE
)
FROM payment_transactions;

SELECT setval(
    pg_get_serial_sequence(
        'park_entries',
        'entry_id'
    ),
    COALESCE(MAX(entry_id), 1),
    TRUE
)
FROM park_entries;

SELECT setval(
    pg_get_serial_sequence(
        'ride_usage',
        'usage_id'
    ),
    COALESCE(MAX(usage_id), 1),
    TRUE
)
FROM ride_usage;

SELECT setval(
    pg_get_serial_sequence(
        'maintenance',
        'maintenance_id'
    ),
    COALESCE(MAX(maintenance_id), 1),
    TRUE
)
FROM maintenance;

SELECT setval(
    pg_get_serial_sequence(
        'feedback',
        'feedback_id'
    ),
    COALESCE(MAX(feedback_id), 1),
    TRUE
)
FROM feedback;

-- ============================================================
-- 19. DATABASE CREATION SUCCESS MESSAGE
-- ============================================================

SELECT
    'Amusement Park database created successfully' AS message;

-- ============================================================
-- 20. REQUIRED SELECT AND REPORT QUERIES
-- ============================================================

-- ------------------------------------------------------------
-- QUERY 1: SHOW ALL CUSTOMERS
-- ------------------------------------------------------------

SELECT
    customer_id,
    name,
    phone,
    email,
    age
FROM customers
ORDER BY customer_id;

-- ------------------------------------------------------------
-- QUERY 2: SHOW ACTIVE RIDES AND WAITING TIME
-- ------------------------------------------------------------

SELECT
    ride_id,
    ride_name,
    capacity,
    current_queue_count,
    current_wait_time_minutes
FROM rides
WHERE status = 'ACTIVE'
ORDER BY current_wait_time_minutes DESC;

-- ------------------------------------------------------------
-- QUERY 3: SHOW PAID TICKETS
-- ------------------------------------------------------------

SELECT
    ticket_id,
    customer_id,
    visit_date,
    ticket_type,
    ticket_price
FROM tickets
WHERE payment_status = 'PAID'
ORDER BY visit_date, ticket_id;

-- ------------------------------------------------------------
-- QUERY 4: JOIN RIDES WITH ZONES
-- ------------------------------------------------------------

SELECT
    r.ride_id,
    r.ride_name,
    z.zone_name,
    r.capacity,
    r.status
FROM rides AS r
JOIN zones AS z
    ON z.zone_id = r.zone_id
ORDER BY
    z.zone_name,
    r.ride_name;

-- ------------------------------------------------------------
-- QUERY 5: CUSTOMER TICKET DETAILS
-- ------------------------------------------------------------

SELECT
    t.ticket_id,
    c.name AS customer_name,
    t.visit_date,
    t.ticket_type,
    t.ticket_price,
    t.payment_status
FROM tickets AS t
JOIN customers AS c
    ON c.customer_id = t.customer_id
ORDER BY
    t.visit_date,
    c.name;

-- ------------------------------------------------------------
-- QUERY 6: QR ENTRY VERIFICATION HISTORY
-- ------------------------------------------------------------

SELECT
    pe.entry_id,
    t.qr_code,
    c.name AS customer_name,
    pe.gate_name,
    pe.entry_time,
    pe.exit_time,
    pe.status
FROM park_entries AS pe
JOIN tickets AS t
    ON t.ticket_id = pe.ticket_id
JOIN customers AS c
    ON c.customer_id = t.customer_id
ORDER BY pe.entry_time;

-- ------------------------------------------------------------
-- QUERY 7: COMPLETE RIDE USAGE HISTORY
-- ------------------------------------------------------------

SELECT
    ru.usage_id,
    c.name AS customer_name,
    r.ride_name,
    z.zone_name,
    ru.entry_time
FROM ride_usage AS ru
JOIN customers AS c
    ON c.customer_id = ru.customer_id
JOIN rides AS r
    ON r.ride_id = ru.ride_id
JOIN zones AS z
    ON z.zone_id = r.zone_id
ORDER BY ru.entry_time;

-- ------------------------------------------------------------
-- QUERY 8: TOTAL RIDES USED BY EACH CUSTOMER
-- ------------------------------------------------------------

SELECT
    c.customer_id,
    c.name,
    COUNT(ru.usage_id) AS total_rides_used
FROM customers AS c
LEFT JOIN ride_usage AS ru
    ON ru.customer_id = c.customer_id
GROUP BY
    c.customer_id,
    c.name
ORDER BY
    total_rides_used DESC,
    c.name;

-- ------------------------------------------------------------
-- QUERY 9: RIDE-WISE CUSTOMER COUNT
-- ------------------------------------------------------------

SELECT
    r.ride_id,
    r.ride_name,
    COUNT(ru.usage_id) AS total_ride_entries,
    COUNT(DISTINCT ru.customer_id) AS unique_customers
FROM rides AS r
LEFT JOIN ride_usage AS ru
    ON ru.ride_id = r.ride_id
GROUP BY
    r.ride_id,
    r.ride_name
ORDER BY
    total_ride_entries DESC,
    r.ride_name;

-- ------------------------------------------------------------
-- QUERY 10: MOST POPULAR RIDE REPORT
-- ------------------------------------------------------------

SELECT
    r.ride_id,
    r.ride_name,
    z.zone_name,
    COUNT(ru.usage_id) AS total_ride_entries,
    COUNT(DISTINCT ru.customer_id) AS unique_riders
FROM rides AS r
JOIN zones AS z
    ON z.zone_id = r.zone_id
JOIN ride_usage AS ru
    ON ru.ride_id = r.ride_id
GROUP BY
    r.ride_id,
    r.ride_name,
    z.zone_name
ORDER BY
    total_ride_entries DESC,
    unique_riders DESC
LIMIT 1;

-- ------------------------------------------------------------
-- QUERY 11: REVENUE REPORT
-- ------------------------------------------------------------

SELECT
    visit_date,
    COUNT(*) AS paid_ticket_count,
    SUM(ticket_price) AS total_revenue,
    ROUND(
        AVG(ticket_price),
        2
    ) AS average_ticket_value
FROM tickets
WHERE payment_status = 'PAID'
GROUP BY visit_date
ORDER BY visit_date;

-- ------------------------------------------------------------
-- QUERY 12: REVENUE BY TICKET TYPE
-- ------------------------------------------------------------

SELECT
    ticket_type,
    COUNT(*) AS tickets_sold,
    SUM(ticket_price) AS revenue
FROM tickets
WHERE payment_status = 'PAID'
GROUP BY ticket_type
ORDER BY revenue DESC;

-- ------------------------------------------------------------
-- QUERY 13: DAILY VISITOR REPORT
-- ------------------------------------------------------------

SELECT
    DATE(pe.entry_time) AS visit_day,
    COUNT(DISTINCT pe.ticket_id) AS verified_visitors,
    COUNT(*) AS total_gate_scans
FROM park_entries AS pe
WHERE pe.status IN (
    'ENTERED',
    'EXITED'
)
GROUP BY DATE(pe.entry_time)
ORDER BY visit_day;

-- ------------------------------------------------------------
-- QUERY 14: PEAK-HOUR VISITOR REPORT
-- ------------------------------------------------------------

SELECT
    DATE(entry_time) AS visit_day,
    EXTRACT(HOUR FROM entry_time) AS entry_hour,
    COUNT(*) AS visitor_count
FROM park_entries
WHERE status IN (
    'ENTERED',
    'EXITED'
)
GROUP BY
    DATE(entry_time),
    EXTRACT(HOUR FROM entry_time)
ORDER BY
    visit_day,
    visitor_count DESC;

-- ------------------------------------------------------------
-- QUERY 15: MAINTENANCE HISTORY REPORT
-- ------------------------------------------------------------

SELECT
    m.maintenance_id,
    r.ride_name,

    COALESCE(
        s.name,
        'Not assigned'
    ) AS assigned_staff,

    m.issue_description,
    m.maintenance_date,
    m.status,
    m.completed_at
FROM maintenance AS m
JOIN rides AS r
    ON r.ride_id = m.ride_id
LEFT JOIN staff AS s
    ON s.staff_id = m.staff_id
ORDER BY m.maintenance_date DESC;

-- ------------------------------------------------------------
-- QUERY 16: SHOW ALL UNAVAILABLE RIDES
-- ------------------------------------------------------------

SELECT
    r.ride_id,
    r.ride_name,
    z.zone_name,
    r.status
FROM rides AS r
JOIN zones AS z
    ON z.zone_id = r.zone_id
WHERE r.status IN (
    'CLOSED',
    'UNDER_MAINTENANCE'
)
ORDER BY r.ride_name;

-- ------------------------------------------------------------
-- QUERY 17: GROUP BY + HAVING
-- RIDES WITH AT LEAST TWO SCANS
-- ------------------------------------------------------------

SELECT
    r.ride_name,
    COUNT(ru.usage_id) AS usage_count
FROM rides AS r
JOIN ride_usage AS ru
    ON ru.ride_id = r.ride_id
GROUP BY
    r.ride_id,
    r.ride_name
HAVING COUNT(ru.usage_id) >= 2
ORDER BY usage_count DESC;

-- ------------------------------------------------------------
-- QUERY 18: CUSTOMER SATISFACTION REPORT
-- ------------------------------------------------------------

SELECT
    r.ride_name,
    COUNT(f.feedback_id) AS feedback_count,

    ROUND(
        AVG(f.rating),
        2
    ) AS average_rating

FROM rides AS r
JOIN feedback AS f
    ON f.ride_id = r.ride_id
GROUP BY
    r.ride_id,
    r.ride_name
HAVING AVG(f.rating) >= 4.00
ORDER BY
    average_rating DESC,
    feedback_count DESC;

-- ------------------------------------------------------------
-- QUERY 19: CUSTOMERS WHO NEVER PURCHASED A TICKET
-- ------------------------------------------------------------

SELECT
    c.customer_id,
    c.name,
    c.email
FROM customers AS c
LEFT JOIN tickets AS t
    ON t.customer_id = c.customer_id
WHERE t.ticket_id IS NULL
ORDER BY c.name;

-- ------------------------------------------------------------
-- QUERY 20: CUSTOMERS WITH MULTIPLE PAID TICKETS
-- ------------------------------------------------------------

SELECT
    c.customer_id,
    c.name,
    COUNT(t.ticket_id) AS paid_ticket_count,
    SUM(t.ticket_price) AS total_spent
FROM customers AS c
JOIN tickets AS t
    ON t.customer_id = c.customer_id
WHERE t.payment_status = 'PAID'
GROUP BY
    c.customer_id,
    c.name
HAVING COUNT(t.ticket_id) > 1
ORDER BY total_spent DESC;
