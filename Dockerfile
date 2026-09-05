FROM node:20-bookworm-slim AS builder

WORKDIR /app

COPY Backend/package*.json ./
RUN npm install

COPY Backend/ ./

RUN npm run prisma:generate
RUN npm run build


FROM node:20-bookworm-slim AS runner

WORKDIR /app

ENV NODE_ENV=production

COPY Backend/package*.json ./
RUN npm install --omit=dev

COPY --from=builder /app/dist ./dist
COPY --from=builder /app/generated ./generated
COPY --from=builder /app/prisma ./prisma

EXPOSE 5000

CMD ["sh", "-c", "npx prisma db push --schema prisma/schema.prisma && node dist/prisma/seed.js && node dist/src/server.js"]