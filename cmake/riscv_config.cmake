# RISC-V specific configuration
# Only enable algorithms that work properly on RISC-V

if (XMRIG_RISCV)
    message(STATUS "Configuring for RISC-V architecture")
    
    # Disable algorithms that require x86 intrinsics
    set(WITH_CN_LITE OFF CACHE BOOL "CryptoNight-Lite disabled on RISC-V" FORCE)
    set(WITH_CN_HEAVY OFF CACHE BOOL "CryptoNight-Heavy disabled on RISC-V" FORCE)
    set(WITH_CN_PICO OFF CACHE BOOL "CryptoNight-Pico disabled on RISC-V" FORCE)
    set(WITH_CN_FEMTO OFF CACHE BOOL "CryptoNight-UPX2 disabled on RISC-V" FORCE)
    set(WITH_KAWPOW OFF CACHE BOOL "KawPow disabled on RISC-V" FORCE)
    set(WITH_GHOSTRIDER OFF CACHE BOOL "GhostRider disabled on RISC-V (requires x86 intrinsics)" FORCE)
    set(WITH_ARGON2 OFF CACHE BOOL "Argon2 disabled on RISC-V" FORCE)
    
    # Keep RandomX enabled - this is the main algorithm for RISC-V
    set(WITH_RANDOMX ON CACHE BOOL "RandomX enabled - primary algorithm for RISC-V" FORCE)
    
    # Disable x86-specific optimizations
    set(WITH_ASM OFF CACHE BOOL "ASM disabled on RISC-V" FORCE)
    set(WITH_SSE4_1 OFF CACHE BOOL "SSE4.1 disabled on RISC-V" FORCE)
    set(WITH_AVX2 OFF CACHE BOOL "AVX2 disabled on RISC-V" FORCE)
    set(WITH_VAES OFF CACHE BOOL "VAES disabled on RISC-V" FORCE)
    
    # Disable MSR (x86 specific)
    set(WITH_MSR OFF CACHE BOOL "MSR disabled on RISC-V" FORCE)
    
    # GPU backends not available on typical RISC-V systems
    set(WITH_OPENCL OFF CACHE BOOL "OpenCL disabled on RISC-V" FORCE)
    set(WITH_CUDA OFF CACHE BOOL "CUDA disabled on RISC-V" FORCE)
    set(WITH_NVML OFF CACHE BOOL "NVML disabled on RISC-V" FORCE)
    set(WITH_ADL OFF CACHE BOOL "ADL disabled on RISC-V" FORCE)
    
    # Try to keep useful features
    set(WITH_HWLOC ON CACHE BOOL "HWLOC enabled for CPU topology" FORCE)
    set(WITH_HTTP ON CACHE BOOL "HTTP protocol support enabled" FORCE)
    set(WITH_TLS ON CACHE BOOL "TLS support enabled" FORCE)
    
    message(STATUS "RISC-V configuration: Only RandomX algorithm enabled")
    message(STATUS "Disabled algorithms: CryptoNight variants, KawPow, GhostRider, Argon2")
    message(STATUS "Disabled features: x86 ASM, GPU backends, MSR")
endif()