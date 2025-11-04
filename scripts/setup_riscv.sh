#!/bin/bash
# RISC-V XMRig Setup Script for VisionFive 2 / Orange Pi RV2
# This script prepares your RISC-V system for optimal XMRig performance

set -e

echo "=== XMRig RISC-V Setup Script ==="
echo "Setting up system for optimal mining performance..."

# Check if running on RISC-V
if [ "$(uname -m)" != "riscv64" ]; then
    echo "Warning: This script is designed for RISC-V 64-bit systems"
    echo "Current architecture: $(uname -m)"
fi

# Update system
echo "1. Updating system packages..."
sudo apt-get update
sudo apt-get upgrade -y

# Install dependencies
echo "2. Installing build dependencies..."
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libuv1-dev \
    libssl-dev \
    libhwloc-dev \
    htop \
    cpufrequtils

# Configure huge pages
echo "3. Configuring huge pages..."
echo "Current huge pages: $(cat /proc/sys/vm/nr_hugepages)"

# Calculate optimal huge pages (2GB for full RandomX)
TOTAL_RAM=$(free -g | awk '/^Mem:/{print $2}')
if [ $TOTAL_RAM -ge 4 ]; then
    HUGEPAGES=2048  # 2GB for full mode
    echo "Setting up 2GB huge pages for full RandomX mode"
else
    HUGEPAGES=256   # 256MB for light mode
    echo "Setting up 256MB huge pages for light RandomX mode"
fi

sudo sysctl -w vm.nr_hugepages=$HUGEPAGES
echo "vm.nr_hugepages=$HUGEPAGES" | sudo tee -a /etc/sysctl.conf

# Set CPU governor to performance
echo "4. Setting CPU governor to performance..."
if [ -d "/sys/devices/system/cpu/cpu0/cpufreq" ]; then
    echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
    echo "CPU governor set to performance mode"
else
    echo "No CPU frequency scaling available"
fi

# Create optimized config
echo "5. Creating optimized configuration..."
cat > config.json << 'EOF'
{
  "api": {
    "enabled": false
  },
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS_HERE",
      "pass": "x",
      "tls": true,
      "keepalive": true,
      "coin": "monero"
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "huge-pages-jit": false,
    "hw-aes": null,
    "priority": null,
    "memory-pool": false,
    "yield": true,
    "max-threads-hint": 100,
    "asm": "auto",
    "argon2-impl": "default",
    "cn/0": false,
    "cn-lite/0": false,
    "cn-heavy/0": false,
    "cn-pico": false,
    "cn/upx2": false,
    "ghostrider": false,
    "rx": [0, 2, 4, 6],
    "rx/wow": [0, 2, 4, 6],
    "cn/fast": [0, 1],
    "cn/xao": [0, 1],
    "cn/rto": false,
    "cn-lite/1": false,
    "cn-heavy/tube": false,
    "cn-heavy/xhv": false,
    "cn/ccx": false,
    "kawpow": false,
    "randomx": {
      "init": -1,
      "init-avx2": -1,
      "mode": "auto",
      "1gb-pages": false,
      "rdmsr": false,
      "wrmsr": false,
      "cache_qos": false,
      "numa": true,
      "scratchpad_prefetch_mode": 1
    }
  },
  "opencl": {
    "enabled": false
  },
  "cuda": {
    "enabled": false
  },
  "log-file": "xmrig.log",
  "donate-level": 1,
  "print-time": 60,
  "health-print-time": 60,
  "retries": 5,
  "retry-pause": 5,
  "syslog": false,
  "tls": {
    "enabled": false
  },
  "dns": {
    "ipv6": false,
    "ttl": 30
  },
  "user-agent": null,
  "verbose": 0,
  "watch": true,
  "pause-on-battery": false,
  "pause-on-active": false
}
EOF

echo "Configuration created: config.json"
echo "Remember to replace YOUR_WALLET_ADDRESS_HERE with your actual wallet address"

# System information
echo "6. System Information:"
echo "   Architecture: $(uname -m)"
echo "   Kernel: $(uname -r)"
echo "   CPU cores: $(nproc)"
echo "   Total RAM: ${TOTAL_RAM}GB"
echo "   Huge pages: $HUGEPAGES ($(($HUGEPAGES * 2 / 1024))MB)"
echo "   Available huge pages: $(cat /proc/sys/vm/nr_hugepages)"
echo "   Free huge pages: $(cat /proc/meminfo | grep HugePages_Free | awk '{print $2}')"

# Performance tips
echo ""
echo "=== Performance Tips ==="
echo "1. Use 'htop' to monitor CPU usage during mining"
echo "2. For maximum performance, ensure no other heavy processes are running"
echo "3. Monitor temperature: cat /sys/class/thermal/thermal_zone*/temp"
echo "4. Check mining stats with: ./xmrig --version"
echo ""
echo "=== Next Steps ==="
echo "1. Clone and build: git clone https://github.com/kroryan/xmrig-riscv.git"
echo "2. cd xmrig-riscv && mkdir build && cd build"
echo "3. cmake -DCMAKE_BUILD_TYPE=Release .."
echo "4. make -j$(nproc)"
echo "5. Edit config.json with your wallet address"
echo "6. Run: ./xmrig -c ../config.json"
echo ""
echo "Setup complete! System ready for XMRig RISC-V."