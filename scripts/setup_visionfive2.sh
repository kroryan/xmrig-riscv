#!/bin/bash
# XMRig RISC-V Setup Script - VisionFive 2 Compatible
# This version handles common VisionFive 2 package issues

set -e

echo "=== XMRig RISC-V Setup for VisionFive 2 ==="
echo "Preparing system for mining..."

# Check architecture
if [ "$(uname -m)" != "riscv64" ]; then
    echo "Warning: Expected riscv64, got $(uname -m)"
fi

echo "System: $(uname -a)"

# Fix potential repository issues first
echo "1. Fixing repository configuration..."

# Update package lists, ignore signature errors for now
sudo apt-get update --allow-unauthenticated || echo "Some repositories may have issues, continuing..."

# Install essential packages only
echo "2. Installing essential dependencies..."
sudo apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    pkg-config || echo "Some packages failed, continuing with available ones..."

# Try to install development libraries
echo "3. Installing development libraries..."
sudo apt-get install -y \
    libuv1-dev \
    libssl-dev \
    libhwloc-dev \
    zlib1g-dev || echo "Some dev packages not available, will use system libraries"

# Install monitoring tools
echo "4. Installing monitoring tools..."
sudo apt-get install -y htop curl wget 2>/dev/null || echo "Monitoring tools install skipped"

# Configure huge pages manually
echo "5. Configuring huge pages..."
echo "Current huge pages: $(cat /proc/sys/vm/nr_hugepages 2>/dev/null || echo 0)"

# Calculate RAM and set appropriate huge pages
TOTAL_RAM_KB=$(grep MemTotal /proc/meminfo | awk '{print $2}')
TOTAL_RAM_GB=$((TOTAL_RAM_KB / 1024 / 1024))

echo "Detected RAM: ${TOTAL_RAM_GB}GB"

if [ $TOTAL_RAM_GB -ge 4 ]; then
    HUGEPAGES=1024  # 1GB for full mode on 4GB+ systems
    echo "Setting up 1GB huge pages for full RandomX mode"
elif [ $TOTAL_RAM_GB -ge 2 ]; then
    HUGEPAGES=512   # 512MB for systems with 2GB+
    echo "Setting up 512MB huge pages for light RandomX mode"
else
    HUGEPAGES=128   # 128MB minimum
    echo "Setting up 128MB huge pages (minimum configuration)"
fi

sudo sysctl -w vm.nr_hugepages=$HUGEPAGES
echo "vm.nr_hugepages=$HUGEPAGES" | sudo tee -a /etc/sysctl.conf

# Manual CPU frequency optimization (since cpufrequtils may not be available)
echo "6. CPU frequency optimization..."
if [ -d "/sys/devices/system/cpu/cpu0/cpufreq" ]; then
    echo "CPU frequency scaling detected"
    
    # Show available governors
    if [ -f "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors" ]; then
        AVAILABLE_GOVS=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors)
        echo "Available governors: $AVAILABLE_GOVS"
        
        # Try to set performance governor
        if echo "$AVAILABLE_GOVS" | grep -q "performance"; then
            echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor 2>/dev/null && \
            echo "✓ CPU governor set to performance" || \
            echo "⚠ Failed to set performance governor"
        else
            echo "⚠ Performance governor not available"
        fi
    fi
    
    # Show current frequency settings
    echo "Current governor: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor 2>/dev/null || echo 'unknown')"
    echo "Current frequency: $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq 2>/dev/null || echo 'unknown') kHz"
else
    echo "No CPU frequency scaling available"
fi

# Create basic configuration
echo "7. Creating configuration file..."
cat > config.json << 'EOF'
{
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS_HERE",
      "pass": "visionfive2-worker",
      "tls": true,
      "keepalive": true
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "hw-aes": null,
    "priority": null,
    "yield": true,
    "asm": "auto",
    "rx": [0, 1, 2, 3],
    "rx/wow": [0, 1, 2, 3]
  },
  "randomx": {
    "init": -1,
    "mode": "auto",
    "1gb-pages": false,
    "numa": false
  },
  "log-file": "xmrig.log",
  "print-time": 60,
  "retries": 5,
  "retry-pause": 5
}
EOF

# System information
echo ""
echo "=== System Summary ==="
echo "Architecture: $(uname -m)"
echo "Kernel: $(uname -r)"
echo "CPU cores: $(nproc)"
echo "Total RAM: ${TOTAL_RAM_GB}GB"
echo "Huge pages set: $HUGEPAGES ($(($HUGEPAGES * 2 / 1024))MB)"
echo "Available huge pages: $(cat /proc/sys/vm/nr_hugepages)"

# Check if we can see CPU info
if [ -f "/proc/cpuinfo" ]; then
    CPU_MODEL=$(grep "model name" /proc/cpuinfo | head -1 | cut -d: -f2 | xargs)
    [ -n "$CPU_MODEL" ] && echo "CPU: $CPU_MODEL"
fi

echo ""
echo "=== Next Steps ==="
echo "1. Build XMRig:"
echo "   mkdir build && cd build"
echo "   cmake -DCMAKE_BUILD_TYPE=Release .."
echo "   make -j$(nproc)"
echo ""
echo "2. Edit wallet address:"
echo "   nano config.json"
echo "   # Replace YOUR_WALLET_ADDRESS_HERE with your Monero wallet"
echo ""
echo "3. Test build:"
echo "   ./xmrig --version"
echo ""
echo "4. Run benchmark:"
echo "   ./xmrig --algo=rx/wow --benchmark --bench=100000"
echo ""
echo "5. Start mining:"
echo "   ./xmrig -c ../config.json"
echo ""
echo "Setup complete for VisionFive 2!"