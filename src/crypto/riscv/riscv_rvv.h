/*
 * XMRig RISC-V RVV (Vector Extension) optimizations
 * Copyright (c) 2024 XMRig developers
 * 
 * This file implements vectorized operations using RISC-V Vector Extension (RVV)
 * with scalar fallbacks for better compatibility.
 */

#ifndef XMRIG_RISCV_RVV_H
#define XMRIG_RISCV_RVV_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// RISC-V Vector Extension detection
static inline int riscv_has_rvv() {
#ifdef XMRIG_RVV_ENABLED
    // Check if RVV is available at runtime
    // For now, assume it's available if compiled with RVV support
    return 1;
#else
    return 0;
#endif
}

// Optimized memory operations with RVV
static inline void riscv_memcpy_rvv(void* dest, const void* src, size_t n) {
    if (riscv_has_rvv() && n >= 64) {
#ifdef XMRIG_RVV_ENABLED
        // Use RVV for large copies
        const uint8_t* s = (const uint8_t*)src;
        uint8_t* d = (uint8_t*)dest;
        
        // Process 64-byte chunks with vectors
        while (n >= 64) {
            // Load vector from source
            asm volatile (
                "vsetvli t0, %2, e8, m8, ta, ma\n"
                "vle8.v v0, (%1)\n"
                "vse8.v v0, (%0)\n"
                : 
                : "r"(d), "r"(s), "r"(64)
                : "t0", "memory"
            );
            s += 64;
            d += 64;
            n -= 64;
        }
        
        // Handle remaining bytes with scalar
        if (n > 0) {
            memcpy(d, s, n);
        }
#else
        memcpy(dest, src, n);
#endif
    } else {
        memcpy(dest, src, n);
    }
}

static inline void riscv_memset_rvv(void* dest, int c, size_t n) {
    if (riscv_has_rvv() && n >= 64) {
#ifdef XMRIG_RVV_ENABLED
        uint8_t* d = (uint8_t*)dest;
        
        // Process 64-byte chunks with vectors
        while (n >= 64) {
            asm volatile (
                "vsetvli t0, %2, e8, m8, ta, ma\n"
                "vmv.v.x v0, %1\n"
                "vse8.v v0, (%0)\n"
                : 
                : "r"(d), "r"(c), "r"(64)
                : "t0", "memory"
            );
            d += 64;
            n -= 64;
        }
        
        // Handle remaining bytes with scalar
        if (n > 0) {
            memset(d, c, n);
        }
#else
        memset(dest, c, n);
#endif
    } else {
        memset(dest, c, n);
    }
}

// Vectorized XOR operation
static inline void riscv_xor_rvv(const uint8_t* a, const uint8_t* b, uint8_t* out, size_t n) {
    if (riscv_has_rvv() && n >= 64) {
#ifdef XMRIG_RVV_ENABLED
        // Process 64-byte chunks with vectors
        while (n >= 64) {
            asm volatile (
                "vsetvli t0, %3, e8, m8, ta, ma\n"
                "vle8.v v0, (%0)\n"
                "vle8.v v8, (%1)\n"
                "vxor.vv v0, v0, v8\n"
                "vse8.v v0, (%2)\n"
                : 
                : "r"(a), "r"(b), "r"(out), "r"(64)
                : "t0", "memory"
            );
            a += 64;
            b += 64;
            out += 64;
            n -= 64;
        }
        
        // Handle remaining bytes with scalar
        while (n-- > 0) {
            *out++ = *a++ ^ *b++;
        }
#else
        while (n-- > 0) {
            *out++ = *a++ ^ *b++;
        }
#endif
    } else {
        while (n-- > 0) {
            *out++ = *a++ ^ *b++;
        }
    }
}

// Vectorized memory compare
static inline int riscv_memcmp_rvv(const void* a, const void* b, size_t n) {
    if (riscv_has_rvv() && n >= 64) {
#ifdef XMRIG_RVV_ENABLED
        const uint8_t* pa = (const uint8_t*)a;
        const uint8_t* pb = (const uint8_t*)b;
        
        // Process 64-byte chunks with vectors
        while (n >= 64) {
            int result;
            asm volatile (
                "vsetvli t0, %3, e8, m8, ta, ma\n"
                "vle8.v v0, (%1)\n"
                "vle8.v v8, (%2)\n"
                "vmsne.vv v16, v0, v8\n"
                "vfirst.m %0, v16\n"
                : "=r"(result)
                : "r"(pa), "r"(pb), "r"(64)
                : "t0", "memory"
            );
            
            if (result != -1) {
                // Found difference, do scalar compare for exact result
                return memcmp(pa, pb, 64);
            }
            
            pa += 64;
            pb += 64;
            n -= 64;
        }
        
        // Handle remaining bytes with scalar
        if (n > 0) {
            return memcmp(pa, pb, n);
        }
        return 0;
#else
        return memcmp(a, b, n);
#endif
    } else {
        return memcmp(a, b, n);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* XMRIG_RISCV_RVV_H */