#!/bin/bash

echo "🧹 Cleaning build directory..."
cd ~/dev/repos/github/NovaBank/backend/build
rm -rf *

echo "🔨 Running CMake..."
cmake ..

echo "🏗️ Building NovaBank..."
make

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🚀 Starting server..."
    ./novabank
else
    echo "❌ Build failed. Check the errors above."
fi
