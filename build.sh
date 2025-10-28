#!/bin/bash
# ---------------------------------------------------
# Linux/Mac Build Script for Ashwathama Chess Engine
# ---------------------------------------------------

echo "Building Ashwathama Chess Engine..."

# Compile with g++ (C++17, optimizations enabled)
g++ -std=c++17 -O2 -o engine src/main.cpp -Isrc

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful! Binary: engine"
    echo ""
    echo "To run in UCI mode: ./engine --uci"
    echo "To run normally:    ./engine"
    chmod +x engine
else
    echo ""
    echo "Build failed! Check errors above."
    exit 1
fi
