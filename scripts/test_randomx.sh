#!/bin/bash
# RandomX Testing Script for RISC-V
# Tests different RandomX algorithms and modes

set -e

echo "=== RandomX RISC-V Testing Script ==="
echo "Testing RandomX algorithms on RISC-V architecture"

# Check if xmrig exists
if [ ! -f "./xmrig" ]; then
    echo "Error: xmrig binary not found. Build first:"
    echo "  ./scripts/build_riscv.sh"
    exit 1
fi

# Check architecture
ARCH=$(uname -m)
if [ "$ARCH" != "riscv64" ]; then
    echo "Warning: Not running on riscv64 (detected: $ARCH)"
fi

echo ""
echo "System Information:"
echo "Architecture: $(uname -m)"
echo "CPU cores: $(nproc)"
echo "Available RAM: $(free -h | grep Mem: | awk '{print $2}')"
echo "Huge pages: $(cat /proc/sys/vm/nr_hugepages 2>/dev/null || echo 0)"

# Test binary
echo ""
echo "1. Testing XMRig binary..."
./xmrig --version
echo ""

# Function to run benchmark
run_benchmark() {
    local algo=$1
    local mode=$2
    local duration=$3
    
    echo "Testing $algo ($mode mode) for ${duration}s..."
    
    if [ "$mode" = "light" ]; then
        timeout ${duration} ./xmrig --algo=$algo --randomx-mode=light --benchmark --bench=1M || true
    else
        timeout ${duration} ./xmrig --algo=$algo --randomx-mode=fast --benchmark --bench=1M || true
    fi
    echo ""
}

# Quick tests (30 seconds each)
echo "2. Quick RandomX Algorithm Tests (30s each):"
echo ""

run_benchmark "rx/wow" "light" 30
run_benchmark "rx/wow" "fast" 30  
run_benchmark "rx/0" "light" 30
run_benchmark "rx/0" "fast" 30

# Memory usage test
echo "3. Memory Usage Test:"
echo "Testing light mode memory usage..."
timeout 60 ./xmrig --algo=rx/wow --randomx-mode=light --benchmark --bench=500K &
XMRIG_PID=$!
sleep 10

if ps -p $XMRIG_PID > /dev/null; then
    echo "Memory usage during mining:"
    ps -p $XMRIG_PID -o pid,ppid,%cpu,%mem,vsz,rss,comm
    kill $XMRIG_PID 2>/dev/null || true
fi
wait 2>/dev/null

echo ""
echo "4. Huge Pages Test:"
HUGEPAGES_BEFORE=$(cat /proc/sys/vm/nr_hugepages)
echo "Huge pages before: $HUGEPAGES_BEFORE"

# Test with huge pages if available
if [ "$HUGEPAGES_BEFORE" -gt 100 ]; then
    echo "Testing with huge pages enabled..."
    timeout 30 ./xmrig --algo=rx/wow --huge-pages --benchmark --bench=500K || true
else
    echo "Not enough huge pages configured (need >100, have $HUGEPAGES_BEFORE)"
    echo "To configure huge pages:"
    echo "  sudo sysctl -w vm.nr_hugepages=1050"
fi

echo ""
echo "5. Performance Summary:"
echo ""
echo "Recommended configurations for RISC-V:"
echo ""

if [ "$(free | grep Mem: | awk '{print $2}')" -gt 4000000 ]; then
    echo "✅ 4GB+ RAM detected - Fast mode recommended:"
    echo "   ./xmrig --algo=rx/0 --randomx-mode=fast --threads=3"
    echo ""
else
    echo "⚠️  Limited RAM detected - Light mode recommended:"
    echo "   ./xmrig --algo=rx/wow --randomx-mode=light --threads=3"
    echo ""
fi

CPU_COUNT=$(nproc)
if [ $CPU_COUNT -ge 4 ]; then
    THREADS=$((CPU_COUNT - 1))
    echo "✅ Multi-core CPU detected ($CPU_COUNT cores)"
    echo "   Recommended threads: $THREADS"
else
    echo "⚠️  Limited cores detected ($CPU_COUNT cores)"
    echo "   Recommended threads: $CPU_COUNT"
fi

echo ""
echo "6. Next Steps:"
echo ""
echo "• Edit config file: cp config_riscv.json config.json && nano config.json"
echo "• Add your wallet: Replace YOUR_WALLET_ADDRESS_HERE"
echo "• Start mining: ./xmrig -c config.json"
echo "• Monitor: watch -n 5 'ps aux | grep xmrig'"
echo ""
echo "Popular RandomX pools:"
echo "• pool.minexmr.com:4444 (Monero)"
echo "• wow.miner.rocks:4444 (Wownero - good for testing)"
echo ""
echo "=== RandomX RISC-V Test Complete ==="