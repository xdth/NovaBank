# NovaBank Backend

C++ backend service using Crow framework, SQLite database, and RESTful API design.

## 🚀 Quick Start

### Option 1: Automated Setup
```bash
chmod +x setup.sh
./setup.sh
./build/novabank
```

### Option 2: Manual Setup

#### Prerequisites
- C++17 compatible compiler (GCC 8+ or Clang 7+)
- CMake 3.16+
- SQLite3 development files
- OpenSSL development files

#### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    nlohmann-json3-dev \
    libssl-dev
```

#### Build Steps
```bash
mkdir build && cd build
cmake ..
make
./novabank
```

## 🧪 Testing the Server

Once running, test the endpoints:

```bash
# Health check
curl http://localhost:8080/health

# API info
curl http://localhost:8080/api/v1
```

## 🐳 Docker Build

```bash
docker build -t novabank-backend .
docker run -p 8080:8080 novabank-backend
```

## 📁 Project Structure

```
backend/
├── src/
│   ├── main.cpp           # Application entry point
│   ├── api/               # REST API controllers
│   ├── domain/            # Business domain models
│   ├── repository/        # Data access layer
│   ├── db/                # Database management
│   └── utils/             # Utility functions
├── tests/                 # Unit tests
├── CMakeLists.txt         # Build configuration
├── Dockerfile             # Container definition
└── README.md              # This file
```

## 🔧 VS Code Setup

If using VS Code, the `.vscode/c_cpp_properties.json` file is included for proper IntelliSense support. Make sure to:

1. Install the C/C++ extension
2. Build the project once to generate compile_commands.json
3. Reload VS Code window if IntelliSense issues persist

## 🏗️ Development Status

- [x] Basic Crow server setup
- [x] CORS configuration
- [x] Health check endpoint
- [ ] SQLite database integration
- [ ] User authentication
- [ ] Account management
- [ ] Transaction processing
- [ ] Admin endpoints
