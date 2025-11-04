#!/bin/bash
# Quick compilation test for RISC-V fixes

echo "=== Testing RISC-V compilation fixes ==="
echo "Checking if compilation errors are resolved..."

cd ~/xmrig-riscv/build

# Try to continue compilation from where it failed
echo "Continuing compilation..."
make -j1

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compilation SUCCESSFUL!"
    echo "Testing binary..."
    ./xmrig --version
    echo ""
    echo "Quick RandomX test..."
    timeout 10 ./xmrig --algo=rx/wow --benchmark --bench=100 || echo "Test completed"
else
    echo ""
    echo "❌ Compilation still has errors"
    echo "Check the output above for remaining issues"
fi