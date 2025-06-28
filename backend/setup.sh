#!/bin/bash

# NovaBank Backend Setup Script

echo "🏦 Setting up NovaBank Backend Development Environment..."

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    if command -v apt-get &> /dev/null; then
        echo "📦 Installing dependencies for Ubuntu/Debian..."
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            libsqlite3-dev \
            nlohmann-json3-dev \
            libssl-dev \
            pkg-config
    elif command -v yum &> /dev/null; then
        echo "📦 Installing dependencies for RHEL/CentOS..."
        sudo yum install -y \
            gcc-c++ \
            cmake \
            git \
            sqlite-devel \
            openssl-devel
    else
        echo "❌ Unsupported Linux distribution"
        exit 1
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "📦 Installing dependencies for macOS..."
    if ! command -v brew &> /dev/null; then
        echo "❌ Homebrew not found. Please install Homebrew first."
        exit 1
    fi
    brew install cmake sqlite3 nlohmann-json openssl
else
    echo "❌ Unsupported operating system"
    exit 1
fi

# Create build directory
echo "🔨 Creating build directory..."
mkdir -p build

# Create .vscode directory for VS Code users
if [ ! -d ".vscode" ]; then
    echo "📝 Creating VS Code configuration..."
    mkdir -p .vscode
fi

# Build the project
echo "🏗️ Building NovaBank..."
cd build
cmake ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🚀 You can now run: ./build/novabank"
else
    echo "❌ Build failed. Please check the error messages above."
    exit 1
fi
