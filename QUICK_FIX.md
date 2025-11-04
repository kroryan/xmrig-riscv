# Solución Rápida para Errores de Compilación en VisionFive 2

## Error Actual: `-minline-atomics` no reconocido

Tu error se debe a que GCC 11.3 no reconoce algunos flags optimizados. 

### Solución Inmediata:

```bash
# 1. Limpiar build anterior
cd ~/xmrig-riscv
rm -rf build

# 2. Usar configuración compatible
mkdir build && cd build

# 3. CMake con configuración segura para VisionFive 2
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

# 4. Compilar con un solo job para evitar problemas de memoria
make -j1
```

### Si aún hay problemas:

```bash
# Configuración minimalista (sin optimizaciones avanzadas)
cmake -DCMAKE_BUILD_TYPE=Release \
      -DWITH_ASM=OFF \
      -DWITH_HWLOC=OFF \
      -DWITH_TLS=OFF \
      -DWITH_MSR=OFF \
      -DWITH_OPENCL=OFF \
      -DWITH_CUDA=OFF \
      -DWITH_RANDOMX=ON \
      ..

make -j1
```

### Script Automático:

```bash
# Usar el script de build automático
chmod +x scripts/build_riscv.sh
./scripts/build_riscv.sh
```

### Verificar el Build:

```bash
# Si compila exitosamente:
./xmrig --version
# Debería mostrar: XMRig/6.x.x (Linux RISC-V, 64-bit)

# Test rápido:
./xmrig --algo=rx/wow --benchmark --bench=1000
```

### Configuración Final para Mining:

```json
{
  "pools": [
    {
      "url": "pool.minexmr.com:4444",
      "user": "TU_WALLET_AQUI"
    }
  ],
  "cpu": {
    "enabled": true,
    "huge-pages": true,
    "threads": 3,
    "rx/wow": [0, 1, 2]
  }
}
```

### Comando de Mining:

```bash
# Modo conservador para VisionFive 2
./xmrig --algo=rx/wow --threads=3 -o pool.minexmr.com:4444 -u TU_WALLET
```

**Resultado esperado:** 15-25 H/s en VisionFive 2 🎯