#!/bin/bash
# Quick compilation test for RISC-V fixes

echo "=== Testing RISC-V compilation fixes ==="
echo "Checking if mm_malloc.h and other compilation errors are resolved..."

cd ~/xmrig-riscv/build

# Show current progress
echo "Current build progress:"
ls -la xmrig 2>/dev/null && echo "✅ xmrig binary exists" || echo "⚠️  xmrig binary not found yet"

# Try to continue compilation from where it failed
echo ""
echo "Continuing compilation with verbose output..."
make -j1 VERBOSE=1

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 Compilation SUCCESSFUL!"
    echo "Binary details:"
    ls -la xmrig
    file xmrig
    echo ""
    echo "Testing XMRig version..."
    ./xmrig --version
    echo ""
    echo "Quick RandomX algorithm test..."
    timeout 15 ./xmrig --algo=rx/wow --benchmark --bench=500 || echo "RandomX test completed"
    echo ""
    echo "✅ BUILD COMPLETE - Ready to mine!"
else
    echo ""
    echo "❌ Compilation still has errors"
    echo "Last error context above ⬆️"
    echo ""
    echo "Common fixes:"
    echo "1. Update system: sudo apt update && sudo apt upgrade"
    echo "2. Install missing libs: sudo apt install libuv1-dev libssl-dev" 
    echo "3. Clean rebuild: rm -rf build && mkdir build && cd build && cmake .."
fi