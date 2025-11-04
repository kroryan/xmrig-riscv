# XMRig RISC-V Port

This repository contains a RISC-V port of XMRig optimized for RISC-V 64-bit architecture, specifically tested on VisionFive 2 and Orange Pi RV2.

## Key Features

- **RISC-V RVV (Vector Extension) Support**: Vectorized memory operations for improved performance
- **Soft AES Implementation**: No crashes on RISC-V systems without hardware AES
- **Optimized Compiler Flags**: Uses `-march=rv64gcv_zba_zbb_zbc_zbs` for maximum performance
- **Memory Optimizations**: Cache-aware algorithms and huge pages support
- **RISC-V CPU Detection**: Proper CPU topology and feature detection

## Optimizations Included

### Compiler Optimizations
- `-march=rv64gc` - Compatible RISC-V ISA (rv64gcv for advanced systems)
- `-funroll-loops -fomit-frame-pointer -finline-functions` - Performance optimizations
- `-ffast-math -ftree-vectorize` - Mathematical and vectorization optimizations
- `-finline-atomics` - Efficient atomic operations (GCC 11+ compatible)

### RISC-V Specific Code
- `src/crypto/riscv/riscv_rvv.h` - RVV vectorized operations
- `src/crypto/riscv/riscv_crypto.h` - Bit manipulation and crypto helpers
- `src/crypto/riscv/riscv_memory.h` - Memory barriers and atomic operations
- `src/crypto/riscv/RxDataset_riscv.h` - RandomX dataset optimizations

### Performance Features
- **RandomX Soft AES**: Prevents crashes on RISC-V without hardware AES
- **Vectorized Memory Operations**: Uses RVV when available, scalar fallback
- **Cache-Aware Algorithms**: Optimized for typical RISC-V cache hierarchies
- **Huge Pages Support**: 2GB huge pages for optimal memory performance

## Building

### Prerequisites
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libuv1-dev libssl-dev libhwloc-dev
sudo apt-get install gcc g++
```

### Build Instructions

#### Method 1: Automated Script (Recommended)
```bash
git clone https://github.com/kroryan/xmrig-riscv.git
cd xmrig-riscv

# Run automated setup
chmod +x scripts/setup_visionfive2.sh
sudo ./scripts/setup_visionfive2.sh

# Build with automated script
chmod +x scripts/build_riscv.sh
./scripts/build_riscv.sh
```

#### Method 2: Manual Build
```bash
git clone https://github.com/kroryan/xmrig-riscv.git
cd xmrig-riscv

# Enable huge pages (adjust for your system)
sudo sysctl -w vm.nr_hugepages=512  # VisionFive 2
# sudo sysctl -w vm.nr_hugepages=2048  # 8GB+ systems

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_ASM=OFF \
      -DCMAKE_C_FLAGS="-march=rv64gc -O2" \
      -DCMAKE_CXX_FLAGS="-march=rv64gc -O2" \
      ..
make -j1  # Use single job for stability
```

## Performance

### Tested Hardware
- **VisionFive 2**: RISC-V 64-bit, 4-8 cores
- **Orange Pi RV2**: Ky X1 cores, 8 cores

### Expected Performance
- **RandomX**: 30-35 H/s on 8-core RISC-V systems
- **Dataset Init**: ~8 minutes (down from 30+ minutes)
- **Memory Usage**: ~2.3 GB with huge pages
- **Stability**: No crashes, 100% huge page utilization

## Usage

### Basic Configuration

#### Optimized for VisionFive 2
```json
{
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS",
      "pass": "visionfive2-worker",
      "tls": true,
      "keepalive": true
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "hw-aes": null,
    "threads": 3,
    "rx/wow": [0, 1, 2]
  },
  "randomx": {
    "init": -1,
    "mode": "light",
    "1gb-pages": false
  }
}
```

#### Full System Configuration
```json
{
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS",
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
    "asm": "auto"
  },
  "randomx": {
    "init": -1,
    "mode": "auto",
    "1gb-pages": false
  }
}
```

### Performance Tuning
```bash
# Set CPU governor to performance
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Run with optimal settings
./xmrig --algo=rx/0 -c config.json

# For light mode (faster initialization)
./xmrig --algo=rx/wow -c config.json
```

## Troubleshooting

### Common Build Errors

#### Error: `-minline-atomics` not recognized (VisionFive 2)
This occurs with GCC 11.3. **Solution:**

```bash
# 1. Clean previous build
cd ~/xmrig-riscv
rm -rf build && mkdir build && cd build

# 2. Use compatible configuration
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_ASM=OFF \
      -DWITH_SSE4_1=OFF \
      -DWITH_AVX2=OFF \
      -DWITH_VAES=OFF \
      -DWITH_HWLOC=OFF \
      -DWITH_OPENCL=OFF \
      -DWITH_CUDA=OFF \
      -DCMAKE_C_FLAGS="-march=rv64gc -O2" \
      -DCMAKE_CXX_FLAGS="-march=rv64gc -O2" \
      ..

# 3. Build with single job (avoids memory issues)
make -j1
```

#### Minimal Configuration (if above fails)
```bash
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_ASM=OFF \
      -DWITH_HWLOC=OFF \
      -DWITH_TLS=OFF \
      -DWITH_MSR=OFF \
      -DWITH_OPENCL=OFF \
      -DWITH_CUDA=OFF \
      ..

make -j1
```

#### Automated Build Script
```bash
# Use the automated build script
chmod +x scripts/build_riscv.sh
./scripts/build_riscv.sh
```

### Build Issues
- **GCC Version**: Ensure GCC 9+ with RISC-V support
- **Dependencies**: Check CMake finds all libraries
- **Memory**: Use `make -j1` on systems with limited RAM
- **Architecture**: Verify you're on `riscv64` architecture

### Runtime Issues
- **Low Hashrate**: Check CPU governor, enable huge pages
- **Crashes**: Soft AES is automatic on RISC-V (no hardware AES)
- **Slow Init**: Use light mode (`rx/wow`) or ensure sufficient RAM

### Memory Issues
- **Huge Pages**: `sudo sysctl -w vm.nr_hugepages=512` (VisionFive 2)
- **Out of Memory**: Reduce threads or use light mode
- **Fragmentation**: Reboot and set huge pages early

### VisionFive 2 Specific

#### Quick Test Commands
```bash
# Verify build
./xmrig --version
# Should show: XMRig/6.x.x (Linux RISC-V, 64-bit)

# Quick benchmark
./xmrig --algo=rx/wow --benchmark --bench=1000

# Conservative mining
./xmrig --algo=rx/wow --threads=3 -o pool.minexmr.com:4444 -u YOUR_WALLET
```

#### Expected Performance
- **VisionFive 2 (4-core)**: 15-25 H/s (light mode)
- **Dataset Init**: 3-8 minutes
- **Stability**: No crashes with soft AES

## Contributing

This port maintains compatibility with upstream XMRig while adding RISC-V specific optimizations. Contributions are welcome!

## License

Same as upstream XMRig - GPL v3+