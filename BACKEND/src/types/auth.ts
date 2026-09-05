export type AuthRole = "ADMIN" | "STAFF" | "CUSTOMER";

export interface AuthUser {
  id: string;
  email: string;
  role: AuthRole;
}