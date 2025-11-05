# XMRig RISC-V Port - RandomX Optimized

This repository contains a **RandomX-focused** RISC-V port of XMRig, optimized for RISC-V 64-bit architecture. **RandomX** is the primary mining algorithm supported, specifically tested on VisionFive 2 and Orange Pi RV2.

## RandomX Algorithm Support

**RandomX** is a proof-of-work algorithm specifically designed for general-purpose CPUs:
- 🎯 **CPU-optimized**: Uses integer math, floating point operations, and branches
- 💾 **Memory-hard**: Requires significant memory (256MB-2GB) to discourage ASICs
- 🔄 **Virtual machine**: Executes randomized programs in a specialized instruction set
- 🛡️ **ASIC-resistant**: Designed to maintain CPU mining advantage

### Why RandomX Works Best on RISC-V

- ✅ **No x86 intrinsics required** - Pure C++ implementation
- ✅ **Interpreter mode available** - Works without JIT compilation  
- ✅ **IEEE 754 compliant** - Reproducible results across architectures
- ✅ **Memory-focused** - Leverages RISC-V's efficient memory architecture
- ❌ **Other algorithms disabled** - CryptoNight, KawPow, GhostRider need x86 features

## Key RISC-V Features

- **RandomX Algorithm**: Primary mining algorithm (rx/wow, rx/0, rx/loki)
- **Soft AES Implementation**: No crashes on RISC-V systems without hardware AES
- **Optimized Compiler Flags**: Uses `-march=rv64gc` for broad compatibility
- **Memory Optimizations**: Huge pages support and cache-aware algorithms
- **RISC-V CPU Detection**: Proper CPU topology and ISA extension detection

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

#### Method 2: Manual Build (Tested & Working)
```bash
# 1. Navegar al directorio correcto del proyecto
cd ~/xmrig-riscv

# 2. Limpiar cualquier build anterior
rm -rf build

# 3. Crear directorio build limpio
mkdir build && cd build

# 4. Ahora sí, ejecutar cmake (nota el ".." al final)
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

# 5. Compilar
make -j1
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

## RandomX Configuration Guide

### Supported RandomX Algorithms

- **rx/wow** - Wownero (recommended for testing)
- **rx/0** - Monero (main mining target)  
- **rx/loki** - Oxen (formerly Loki)
- **rx/arq** - ArQmA
- **rx/sfx** - Safex

### RandomX Operating Modes

#### Fast Mode (Recommended for 4GB+ RAM)
- **Memory**: ~2080 MiB per mining instance
- **Performance**: Maximum hashrate
- **Initialization**: 3-8 minutes on RISC-V
- **Use case**: Dedicated mining systems

#### Light Mode (For Limited RAM)  
- **Memory**: ~256 MiB per mining instance
- **Performance**: ~50-70% of fast mode
- **Initialization**: 30-60 seconds
- **Use case**: VisionFive 2, shared systems

### Basic Configuration

#### VisionFive 2 - Light Mode (256MB RAM)
```json
{
  "algo": "rx/wow",
  "pools": [
    {
      "url": "pool.minexmr.com:4444", 
      "user": "YOUR_WALLET_ADDRESS",
      "pass": "visionfive2-light",
      "coin": "monero"
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "threads": 3,
    "priority": 2
  },
  "randomx": {
    "init": -1,
    "mode": "light",
    "1gb-pages": false,
    "scratchpad_prefetch_mode": 1
  }
}
```

#### VisionFive 2 - Fast Mode (2GB RAM)
```json
{
  "algo": "rx/0",
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS", 
      "pass": "visionfive2-fast",
      "coin": "monero"
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "threads": 3,
    "priority": 2
  },
  "randomx": {
    "init": -1,
    "mode": "fast",
    "1gb-pages": false,
    "rdmsr": false
  }
}
```

#### High-End RISC-V (8+ cores, 8GB+ RAM)
```json
{
  "algo": "rx/0",
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "YOUR_WALLET_ADDRESS",
      "coin": "monero",
      "tls": true
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "threads": 6,
    "priority": 2,
    "yield": true
  },
  "randomx": {
    "init": -1,
    "mode": "fast", 
    "1gb-pages": false,
    "scratchpad_prefetch_mode": 1,
    "rdmsr": false
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

#### Error: `CMakeLists.txt` not found or configuration errors
This happens when cmake is executed from wrong directory. **Solution:**

```bash
# IMPORTANT: Always follow this exact sequence
cd ~/xmrig-riscv

# Clean any previous build
rm -rf build

# Create fresh build directory  
mkdir build && cd build

# Execute cmake from build directory pointing to parent (..)
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

# Build (use single job to avoid memory issues)
make -j1
```

#### Alternative: Automated Build Script
```bash
# If manual method fails, use the automated script
chmod +x scripts/build_riscv.sh
./scripts/build_riscv.sh
```

#### Directory Structure Check
```bash
# Verify you're in the right place
pwd                    # Should show: /home/username/xmrig-riscv
ls CMakeLists.txt      # Should exist
ls src/                # Should exist
```

#### Post-Build Verification
```bash
# After successful compilation, verify the binary
cd ~/xmrig-riscv/build
ls -la xmrig           # Should show executable file
./xmrig --version      # Should show: XMRig/6.x.x (Linux RISC-V, 64-bit)

# Test RandomX algorithm (built-in benchmark)
./xmrig --algo=rx/wow --bench=1M
```

### Build Issues
- **Wrong Directory**: Always run cmake from `build/` directory pointing to parent `..`
- **Missing Files**: Ensure `CMakeLists.txt` exists in project root
- **GCC Version**: Ensure GCC 9+ with RISC-V support  
- **Memory**: Use `make -j1` on systems with limited RAM
- **Architecture**: Verify you're on `riscv64` architecture with `uname -m`

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
./xmrig --algo=rx/wow --bench=1M

# Conservative mining
./xmrig --algo=rx/wow --threads=3 -o pool.minexmr.com:4444 -u YOUR_WALLET
```

#### RandomX Performance Expectations

**VisionFive 2 (4-core StarFive JH7110)**
- **Light Mode**: 8-12 H/s (rx/wow), 6-10 H/s (rx/0)
- **Fast Mode**: 12-18 H/s (rx/wow), 10-15 H/s (rx/0)  
- **Dataset Init**: 3-8 minutes (fast), 30-60 seconds (light)
- **Memory Usage**: 2GB (fast), 256MB (light)

**Orange Pi RV2 (8-core Ky X1)**
- **Light Mode**: 15-20 H/s (rx/wow), 12-18 H/s (rx/0)
- **Fast Mode**: 25-35 H/s (rx/wow), 20-30 H/s (rx/0)
- **Dataset Init**: 2-5 minutes (fast), 20-40 seconds (light)

### RandomX Benchmark Commands

```bash
# Quick algorithm test (30 seconds each)
./xmrig --algo=rx/wow --bench=1M
./xmrig --algo=rx/0 --bench=1M

# Extended benchmark (10 minutes)
./xmrig --algo=rx/wow --bench=10M

# Test both modes
./xmrig --algo=rx/wow --randomx-mode=light --bench=1M
./xmrig --algo=rx/wow --randomx-mode=fast --bench=1M

# Memory usage test (uses huge pages)
./xmrig --algo=rx/0 --randomx-1gb-pages --bench=1M

#### Tip: Run benchmark without a config

The built-in benchmark doesn't need a config file. Use one of:

```bash
# Monero variant (2 MB per thread)
./xmrig --algo=rx/0 --bench=1M --threads=2 --donate-level=0

# Wownero variant (1 MB per thread)
./xmrig --algo=rx/wow --bench=1M --threads=2 --donate-level=0
```

Valid sizes for `--bench` are 1M … 10M. Plain numbers like `1000` are invalid.

#### Optional: Config-based benchmark

You can also run the benchmark via a config file using the `benchmark` object:

```json
{
  "benchmark": {
    "size": "1M",
    "algo": "rx/0"
  }
}
```

Save this as `config_benchmark.json` and run:

```bash
./xmrig -c config_benchmark.json
```
```

### RandomX Mining Commands

```bash
# Light mode mining (VisionFive 2 safe)
./xmrig --algo=rx/wow --randomx-mode=light --threads=3 \
        -o pool.minexmr.com:4444 -u YOUR_WALLET

# Fast mode mining (if you have 4GB+ RAM)
./xmrig --algo=rx/0 --randomx-mode=fast --threads=3 \
        -o pool.minexmr.com:4444 -u YOUR_WALLET

# Conservative mining with huge pages
sudo sysctl -w vm.nr_hugepages=1050
./xmrig --algo=rx/0 --huge-pages --threads=2 \
        -o pool.minexmr.com:4444 -u YOUR_WALLET
```

### Example: Working pool config (MoneroOcean)

This example uses MoneroOcean on port 10128 (no TLS). Replace YOUR_WALLET with your address.

```json
  GNU nano 6.3                                                                             config.json
{
  "autosave": false,
  "donate-level": 0,
  "algo": "rx/0",
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "threads": 3,
    "priority": 1,
    "affinity": [0, 1, 2]
  },
  "pools": [
    {
      "url": "141.94.96.144:3333",
      "user": "Wallet",
      "pass": "vf2",
      "tls": false,
      "keepalive": true
    }
  ]
}


```

Run with:

```bash
./xmrig -c config.json
```

If your network blocks standard mining ports, try a pool/endpoint on 443 with TLS (e.g., HashVault/SupportXMR) and ensure DNS/IPv6 aren’t causing issues.

## Make xmrig a system-wide command

After building, install the binary to a directory in PATH so `xmrig` works from anywhere:

```bash
# From ~/xmrig-riscv/build
sudo install -m 0755 ./xmrig /usr/local/bin/xmrig

# Verify
which xmrig
xmrig --version
```

Alternative methods:
- Symlink instead of copy:

```bash
sudo ln -sf "$(pwd)/xmrig" /usr/local/bin/xmrig
```

- Add build directory to your PATH:

```bash
echo 'export PATH="$HOME/xmrig-riscv/build:$PATH"' >> ~/.profile
source ~/.profile
```

## Keep mining after closing SSH

Pick one method:

### 1) nohup (quick)
```bash
nohup xmrig -c /home/$USER/xmrig-riscv/build/config.json > /home/$USER/xmrig.log 2>&1 & disown
tail -f /home/$USER/xmrig.log
```

### 2) tmux (re-attach later)
```bash
tmux new -s miner
xmrig -c /home/$USER/xmrig-riscv/build/config.json
# Detach: Ctrl+b then d
# Re-attach: tmux attach -t miner
```

### 3) systemd (user service)

```bash
mkdir -p ~/.config/systemd/user
cat > ~/.config/systemd/user/xmrig.service <<'EOF'
[Unit]
Description=XMRig miner (user)
After=network-online.target

[Service]
WorkingDirectory=/home/%u/xmrig-riscv/build
ExecStart=/usr/local/bin/xmrig -c /home/%u/xmrig-riscv/build/config.json
Restart=always
RestartSec=5
Nice=10
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=default.target
EOF

loginctl enable-linger $USER
systemctl --user daemon-reload
systemctl --user enable --now xmrig.service
systemctl --user status xmrig.service

# View logs
journalctl --user -u xmrig.service -f
```

## About (summary)

XMRig is a high performance, open source, cross-platform miner supporting RandomX (CPU), KawPow (GPU), CryptoNight and GhostRider. This RISC-V port focuses on RandomX CPU mining and includes a built-in RandomX benchmark.

Upstream project: https://github.com/xmrig/xmrig

Highlights:
- CPU mining backend (this port targets RISC-V)
- JSON configuration and runtime reload
- HTTP API
- Built-in RandomX benchmark (`--bench=1M..10M`)

Donation:
- Default donate-level is 1%; set `"donate-level": 0` in config to disable.

## Algorithm Support Status

### ✅ Supported (RISC-V Compatible)
- **RandomX family**: rx/0 (Monero), rx/wow (Wownero), rx/loki, rx/arq, rx/sfx
- **Pure C++ implementation**: No platform-specific intrinsics required
- **Cross-platform**: IEEE 754 compliant floating-point operations

### ❌ Disabled (Require x86 intrinsics)  
- **CryptoNight variants**: cn/r, cn/fast, cn/half, cn/2, cn-lite/*
- **KawPow**: Ethereum ProgPoW variant (GPU-optimized)
- **GhostRider**: RTM multi-algorithm (x86 SIMD required)
- **Argon2**: Memory-hard but uses x86 optimizations

### 🔧 RISC-V Technical Details
- **RandomX Mode**: Interpreter-based (no native RISC-V JIT compiler yet)
- **AES Implementation**: Software-only (hardware AES not required)
- **Vector Extensions**: RVV optimizations for memory operations
- **Performance**: ~60-80% of x86 equivalent (limited by interpreter mode)

## Contributing

This port maintains compatibility with upstream XMRig while focusing on RandomX optimization for RISC-V. Contributions welcome, especially:
- Native RISC-V JIT compiler for RandomX
- Further RVV (RISC-V Vector) optimizations
- Additional RISC-V hardware testing

## References

- **RandomX Algorithm**: https://github.com/tevador/RandomX
- **RandomX RISC-V Port**: https://github.com/hadi-guang/RandomX-RISCV
- **Monero Mining Pools**: https://miningpoolstats.stream/monero
- **Wownero (testing)**: https://wownero.org

## License

Same as upstream XMRig - GPL v3+