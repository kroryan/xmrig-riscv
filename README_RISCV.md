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
- `-march=rv64gcv_zba_zbb_zbc_zbs` - Full RISC-V ISA with vector and bit manipulation
- `-funroll-loops -fomit-frame-pointer -finline-functions` - Performance optimizations
- `-ffast-math -ftree-vectorize` - Mathematical and vectorization optimizations
- `-minline-atomics` - Efficient atomic operations

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
```bash
git clone https://github.com/kroryan/xmrig-riscv.git
cd xmrig-riscv

# Enable huge pages (2GB recommended)
sudo sysctl -w vm.nr_hugepages=2048
echo 'vm.nr_hugepages=2048' | sudo tee -a /etc/sysctl.conf

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
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

### Build Issues
- Ensure you have GCC 9+ with RISC-V support
- Check that CMake finds all dependencies
- Verify RISC-V toolchain is properly installed

### Runtime Issues
- **Low Hashrate**: Check CPU governor, enable huge pages
- **Crashes**: Verify soft AES is enabled (automatic on RISC-V)
- **Slow Init**: Use light mode or ensure sufficient RAM

### Memory Issues
- **Huge Pages**: `echo 2048 | sudo tee /proc/sys/vm/nr_hugepages`
- **Out of Memory**: Reduce thread count or use light mode
- **Fragmentation**: Reboot and set huge pages early

## Contributing

This port maintains compatibility with upstream XMRig while adding RISC-V specific optimizations. Contributions are welcome!

## License

Same as upstream XMRig - GPL v3+