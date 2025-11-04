# VisionFive 2 Troubleshooting Guide

## Repository Issues (GPG Key Expired)

If you get GPG signature errors:

```bash
# Option 1: Ignore signatures temporarily
sudo apt-get update --allow-unauthenticated

# Option 2: Update GPG keys
sudo apt-get install debian-archive-keyring
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys E852514F5DF312F6

# Option 3: Use main Debian repos instead of ports (if available)
sudo nano /etc/apt/sources.list
# Comment out ports.debian.org lines
# Add: deb http://deb.debian.org/debian unstable main
```

## Missing Packages on RISC-V

Some packages may not be available. Here are alternatives:

### cpufrequtils not available:
```bash
# Use manual frequency control
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Or check what's available:
ls /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors
```

### Development libraries missing:
```bash
# Use system libraries if dev packages unavailable
export PKG_CONFIG_PATH=/usr/lib/riscv64-linux-gnu/pkgconfig

# Or compile without optional features:
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_HWLOC=OFF ..
```

## Compilation Issues

### GCC version problems:
```bash
# Check GCC version
gcc --version

# If too old, try:
sudo apt-get install gcc-10 g++-10
export CC=gcc-10
export CXX=g++-10
```

### Missing dependencies:
```bash
# Install minimal deps manually:
sudo apt-get install build-essential cmake git
sudo apt-get install libssl-dev || echo "Using system SSL"
sudo apt-get install libuv1-dev || echo "Using bundled libuv"
```

### CMake configuration issues:
```bash
# Clear cache and reconfigure
rm -rf CMakeCache.txt CMakeFiles/
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_HWLOC=OFF \
      -DWITH_TLS=OFF \
      ..
```

## Runtime Issues

### Low hashrate on VisionFive 2:
```bash
# Check CPU governor
cat /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Set to performance if available
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Check huge pages
cat /proc/sys/vm/nr_hugepages
# Should be > 0

# Monitor temperature
cat /sys/class/thermal/thermal_zone*/temp
```

### Memory issues:
```bash
# Check available memory
free -h

# For VisionFive 2 with 4GB RAM, use light mode:
./xmrig --algo=rx/wow -c config.json

# Reduce huge pages if OOM:
sudo sysctl -w vm.nr_hugepages=256
```

### No CPU frequency scaling:
Some VisionFive 2 systems don't have cpufreq. This is normal:
```bash
# Check if available
ls /sys/devices/system/cpu/cpu0/cpufreq/ 2>/dev/null || echo "No cpufreq"

# If not available, the CPU runs at fixed frequency
# Performance should still be good
```

## VisionFive 2 Specific Commands

### Check system info:
```bash
# Architecture
uname -a

# CPU info
cat /proc/cpuinfo | grep -E "(processor|model name|isa)"

# Memory info
cat /proc/meminfo | head -10

# Temperature monitoring
watch -n 1 'cat /sys/class/thermal/thermal_zone*/temp'
```

### Quick setup for VisionFive 2:
```bash
# Use the VisionFive 2 specific script
chmod +x scripts/setup_visionfive2.sh
sudo ./scripts/setup_visionfive2.sh

# Manual quick setup:
sudo sysctl -w vm.nr_hugepages=512
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor 2>/dev/null || echo "No cpufreq available"
```

### Expected performance:
- **4-core VisionFive 2**: 15-25 H/s (light mode)
- **8-core VisionFive 2**: 25-35 H/s (light mode)
- **Dataset init time**: 3-8 minutes
- **Memory usage**: 300-600MB (light), 2GB+ (full)

### Safe mining command for VisionFive 2:
```bash
# Conservative settings for stability
./xmrig --algo=rx/wow --threads=3 --huge-pages=true -o pool.minexmr.com:4444 -u YOUR_WALLET

# Monitor with:
htop  # CPU usage
cat /sys/class/thermal/thermal_zone*/temp  # Temperature
```

## Emergency Recovery

If the system becomes unresponsive:

```bash
# Reduce load immediately:
killall xmrig

# Reset huge pages:
sudo sysctl -w vm.nr_hugepages=0

# Cool down system:
# Wait 5-10 minutes before restarting
```