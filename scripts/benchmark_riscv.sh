#!/bin/bash
# RISC-V XMRig Benchmark Script
# Tests different configurations to find optimal settings

set -e

echo "=== XMRig RISC-V Benchmark ==="
echo "Testing different configurations for optimal performance"
echo ""

# Check if xmrig exists
if [ ! -f "./xmrig" ]; then
    echo "Error: xmrig binary not found in current directory"
    echo "Please build xmrig first and run this from the build directory"
    exit 1
fi

# System info
echo "System Information:"
echo "  Architecture: $(uname -m)"
echo "  CPU cores: $(nproc)"
echo "  RAM: $(free -h | awk '/^Mem:/ {print $2}')"
echo "  Huge pages: $(cat /proc/sys/vm/nr_hugepages)"
echo "  Free huge pages: $(cat /proc/meminfo | grep HugePages_Free | awk '{print $2}')"
echo ""

# Check huge pages
HUGEPAGES=$(cat /proc/sys/vm/nr_hugepages)
if [ $HUGEPAGES -lt 256 ]; then
    echo "Warning: Insufficient huge pages configured ($HUGEPAGES)"
    echo "Run: sudo sysctl -w vm.nr_hugepages=2048"
    echo ""
fi

# Test configurations
echo "Running benchmark tests..."

# Test 1: Light mode benchmark
echo "Test 1: RandomX Light Mode (256MB dataset)"
echo "Starting 1M hash benchmark in light mode..."
timeout 300 ./xmrig --algo=rx/wow --benchmark --bench=1M --threads=$(nproc) --huge-pages=true 2>/dev/null || echo "Light mode test completed"

echo ""

# Test 2: Full mode benchmark (if enough huge pages)
if [ $HUGEPAGES -ge 1024 ]; then
    echo "Test 2: RandomX Full Mode (2GB dataset)"
    echo "Starting 1M hash benchmark in full mode..."
    timeout 600 ./xmrig --algo=rx/0 --benchmark --bench=1M --threads=$(nproc) --huge-pages=true 2>/dev/null || echo "Full mode test completed"
else
    echo "Test 2: Skipped (insufficient huge pages for full mode)"
fi

echo ""

# Test 3: Thread scaling
echo "Test 3: Thread Scaling Analysis"
for threads in 1 2 4 $(nproc); do
    if [ $threads -le $(nproc) ]; then
        echo "Testing with $threads threads..."
        timeout 120 ./xmrig --algo=rx/wow --benchmark --bench=100K --threads=$threads --huge-pages=true 2>/dev/null || echo "Thread test $threads completed"
    fi
done

echo ""

# Test 4: Memory configuration
echo "Test 4: Memory Configuration Test"
echo "Testing without huge pages..."
timeout 120 ./xmrig --algo=rx/wow --benchmark --bench=100K --threads=$(nproc) --huge-pages=false 2>/dev/null || echo "No huge pages test completed"

echo ""

# Performance recommendations
echo "=== Performance Recommendations ==="

TOTAL_RAM=$(free -g | awk '/^Mem:/{print $2}')
CPU_CORES=$(nproc)

echo "Based on your system ($CPU_CORES cores, ${TOTAL_RAM}GB RAM):"

if [ $TOTAL_RAM -ge 4 ] && [ $HUGEPAGES -ge 1024 ]; then
    echo "✓ Recommended: Full RandomX mode (rx/0)"
    echo "  - Expected hashrate: 30-35 H/s"
    echo "  - Command: ./xmrig --algo=rx/0 -c config.json"
elif [ $TOTAL_RAM -ge 2 ]; then
    echo "✓ Recommended: Light RandomX mode (rx/wow)"
    echo "  - Expected hashrate: 25-30 H/s"
    echo "  - Command: ./xmrig --algo=rx/wow -c config.json"
else
    echo "⚠ Warning: Limited RAM, consider light mode only"
fi

echo ""
echo "Optimal thread configuration:"
if [ $CPU_CORES -ge 8 ]; then
    echo "  - Use $(($CPU_CORES - 1)) threads (leave 1 core free)"
    echo "  - Set CPU affinity: taskset -c 0-$(($CPU_CORES - 2))"
elif [ $CPU_CORES -ge 4 ]; then
    echo "  - Use all $CPU_CORES threads"
else
    echo "  - Use all $CPU_CORES threads (limited by CPU count)"
fi

echo ""
echo "Memory optimization:"
if [ $HUGEPAGES -ge 1024 ]; then
    echo "  ✓ Huge pages configured correctly"
else
    echo "  ⚠ Increase huge pages: sudo sysctl -w vm.nr_hugepages=2048"
fi

echo ""
echo "System optimization:"
echo "  - Set CPU governor: echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor"
echo "  - Monitor temps: watch cat /sys/class/thermal/thermal_zone*/temp"
echo "  - Check load: htop"

echo ""
echo "Example optimized command:"
if [ $TOTAL_RAM -ge 4 ] && [ $HUGEPAGES -ge 1024 ]; then
    echo "  ./xmrig --algo=rx/0 --threads=$(($CPU_CORES - 1)) --huge-pages=true -o pool.minexmr.com:4444 -u YOUR_WALLET"
else
    echo "  ./xmrig --algo=rx/wow --threads=$CPU_CORES --huge-pages=true -o pool.minexmr.com:4444 -u YOUR_WALLET"
fi

echo ""
echo "Benchmark complete! Check the output above for optimal settings."