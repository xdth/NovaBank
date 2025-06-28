# Build stage
FROM debian:stable-slim AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libsqlite3-dev \
    nlohmann-json3-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY . .

# Build the application
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# Runtime stage
FROM debian:stable-slim

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Create non-root user
RUN useradd -m -u 1000 novabank

# Set working directory
WORKDIR /app

# Copy built executable from builder
COPY --from=builder /app/build/novabank /app/
COPY --from=builder /app/src/db/migrations.sql /app/

# Create directory for database
RUN mkdir -p /app/data && chown -R novabank:novabank /app

# Switch to non-root user
USER novabank

# Expose port
EXPOSE 8080

# Run the application
CMD ["./novabank"]
