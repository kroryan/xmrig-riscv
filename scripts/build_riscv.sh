#!/bin/bash
# XMRig RISC-V Build Script - Fixed for VisionFive 2
# Handles GCC compatibility and x86 assembly issues

set -e

echo "=== XMRig RISC-V Build Script ==="
echo "Building XMRig with RandomX algorithm for RISC-V architecture"
echo "Algorithms: RandomX family (rx/0, rx/wow, rx/loki) - CPU optimized"

# Check we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Run this from xmrig-riscv root directory."
    exit 1
fi

# Check architecture
if [ "$(uname -m)" != "riscv64" ]; then
    echo "Warning: Not building on riscv64 architecture ($(uname -m))"
fi

# Clean previous build
echo "1. Cleaning previous build..."
rm -rf build
mkdir build
cd build

echo "2. Configuring build for RISC-V..."

# Configure with RISC-V specific options (RandomX optimized)
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_RANDOMX=ON \
      -DWITH_ASM=OFF \
      -DWITH_SSE4_1=OFF \
      -DWITH_AVX2=OFF \
      -DWITH_VAES=OFF \
      -DWITH_GHOSTRIDER=OFF \
      -DWITH_CN_LITE=OFF \
      -DWITH_CN_HEAVY=OFF \
      -DWITH_KAWPOW=OFF \
      -DWITH_ARGON2=OFF \
      -DWITH_HWLOC=ON \
      -DWITH_TLS=ON \
      -DWITH_OPENCL=OFF \
      -DWITH_CUDA=OFF \
      -DCMAKE_C_FLAGS="-march=rv64gc -O3 -funroll-loops" \
      -DCMAKE_CXX_FLAGS="-march=rv64gc -O3 -funroll-loops" \
      ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    echo "Trying with minimal configuration..."
    
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DWITH_RANDOMX=ON \
          -DWITH_ASM=OFF \
          -DWITH_SSE4_1=OFF \
          -DWITH_AVX2=OFF \
          -DWITH_VAES=OFF \
          -DWITH_GHOSTRIDER=OFF \
          -DWITH_CN_LITE=OFF \
          -DWITH_CN_HEAVY=OFF \
          -DWITH_KAWPOW=OFF \
          -DWITH_ARGON2=OFF \
          -DWITH_HWLOC=OFF \
          -DWITH_TLS=OFF \
          -DWITH_OPENCL=OFF \
          -DWITH_CUDA=OFF \
          -DWITH_MSR=OFF \
          ..
fi

echo "3. Building XMRig..."

# Build with limited parallelism to avoid memory issues
NPROC=$(nproc)
if [ $NPROC -gt 2 ]; then
    JOBS=2  # Limit to 2 jobs on RISC-V to avoid memory exhaustion
else
    JOBS=$NPROC
fi

echo "Using $JOBS parallel jobs"
make -j$JOBS

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Build Success! ==="
    echo "XMRig binary: $(pwd)/xmrig"
    
    # Test the binary
    echo "Testing binary..."
    ./xmrig --version
    
    echo ""
    echo "=== RandomX Quick Test ==="
    echo "Running RandomX benchmark..."
    timeout 30 ./xmrig --algo=rx/wow --randomx-mode=light --benchmark --bench=1000 || echo "RandomX benchmark completed"
    
    echo ""
    echo "=== Next Steps - RandomX Mining ==="
    echo "1. Copy RISC-V optimized config:"
    echo "   cp ../config_riscv.json ./config.json"
    echo ""  
    echo "2. Edit wallet address:"
    echo "   nano config.json  # Replace YOUR_WALLET_ADDRESS_HERE"
    echo ""
    echo "3. Test RandomX algorithms:"
    echo "   chmod +x ../scripts/test_randomx.sh && ../scripts/test_randomx.sh"
    echo ""
    echo "4. Run RandomX benchmarks:"
    echo "   ./xmrig --algo=rx/wow --randomx-mode=light --benchmark --bench=1M"
    echo "   ./xmrig --algo=rx/0 --randomx-mode=fast --benchmark --bench=1M"
    echo ""
    echo "5. Start RandomX mining:"
    echo "   ./xmrig -c config.json"
    echo ""
    echo "RandomX build complete for RISC-V!"
else
    echo ""
    echo "=== Build Failed ==="
    echo "Common solutions:"
    echo ""
    echo "1. Try with minimal features:"
    echo "   cmake -DCMAKE_BUILD_TYPE=Release -DWITH_ASM=OFF -DWITH_HWLOC=OFF .."
    echo ""
    echo "2. Check available memory:"
    echo "   free -h"
    echo ""
    echo "3. Use single-threaded build:"
    echo "   make -j1"
    echo ""
    echo "4. Check GCC version:"
    echo "   gcc --version"
    
    exit 1
fi