/*
 * XMRig RISC-V memory optimizations
 * Copyright (c) 2024 XMRig developers
 * 
 * This file implements RISC-V specific memory operations including
 * memory barriers, prefetch, and atomic operations.
 */

#ifndef XMRIG_RISCV_MEMORY_H
#define XMRIG_RISCV_MEMORY_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// RISC-V Atomic extension detection
static inline int riscv_has_atomic() {
#ifdef XMRIG_RISCV_OPTIMIZED
    return 1; // Assume A extension is available if we're optimizing for RISC-V
#else
    return 0;
#endif
}

// Memory barriers
static inline void riscv_memory_barrier() {
    asm volatile ("fence rw, rw" : : : "memory");
}

static inline void riscv_read_barrier() {
    asm volatile ("fence r, rw" : : : "memory");
}

static inline void riscv_write_barrier() {
    asm volatile ("fence rw, w" : : : "memory");
}

// Prefetch operations (using standard RISC-V prefetch hints)
static inline void riscv_prefetch_read(const void* addr) {
#ifdef XMRIG_RISCV_OPTIMIZED
    // Use RISC-V prefetch hint for read access
    asm volatile ("prefetch.r %0" : : "m"(*(const char*)addr));
#else
    (void)addr;
#endif
}

static inline void riscv_prefetch_write(void* addr) {
#ifdef XMRIG_RISCV_OPTIMIZED
    // Use RISC-V prefetch hint for write access  
    asm volatile ("prefetch.w %0" : : "m"(*(char*)addr));
#else
    (void)addr;
#endif
}

static inline void riscv_prefetch_nta(const void* addr) {
#ifdef XMRIG_RISCV_OPTIMIZED
    // Non-temporal access hint (may not pollute cache)
    asm volatile ("prefetch.i %0" : : "m"(*(const char*)addr));
#else
    (void)addr;
#endif
}

// Cache-aware memory copy with prefetching
static inline void riscv_memcpy_prefetch(void* dest, const void* src, size_t n) {
    const char* s = (const char*)src;
    char* d = (char*)dest;
    
    // Prefetch first cache line
    if (n > 0) {
        riscv_prefetch_read(s);
        riscv_prefetch_write(d);
    }
    
    // Copy with prefetching for large blocks
    if (n >= 256) {
        const size_t prefetch_distance = 128;
        
        while (n > prefetch_distance) {
            // Prefetch ahead
            riscv_prefetch_read(s + prefetch_distance);
            riscv_prefetch_write(d + prefetch_distance);
            
            // Copy current block (64 bytes)
            for (int i = 0; i < 64 && n > 0; i++, n--) {
                *d++ = *s++;
            }
        }
    }
    
    // Copy remaining bytes
    while (n-- > 0) {
        *d++ = *s++;
    }
}

// Atomic operations (using A extension if available)
static inline uint32_t riscv_atomic_load32(const volatile uint32_t* ptr) {
    if (riscv_has_atomic()) {
        uint32_t result;
        asm volatile ("lr.w %0, (%1)" : "=r"(result) : "r"(ptr) : "memory");
        return result;
    } else {
        return *ptr;
    }
}

static inline void riscv_atomic_store32(volatile uint32_t* ptr, uint32_t value) {
    if (riscv_has_atomic()) {
        asm volatile ("amoswap.w x0, %1, (%0)" : : "r"(ptr), "r"(value) : "memory");
    } else {
        *ptr = value;
    }
}

static inline uint64_t riscv_atomic_load64(const volatile uint64_t* ptr) {
    if (riscv_has_atomic()) {
        uint64_t result;
        asm volatile ("lr.d %0, (%1)" : "=r"(result) : "r"(ptr) : "memory");
        return result;
    } else {
        return *ptr;
    }
}

static inline void riscv_atomic_store64(volatile uint64_t* ptr, uint64_t value) {
    if (riscv_has_atomic()) {
        asm volatile ("amoswap.d x0, %1, (%0)" : : "r"(ptr), "r"(value) : "memory");
    } else {
        *ptr = value;
    }
}

static inline uint32_t riscv_atomic_add32(volatile uint32_t* ptr, uint32_t value) {
    if (riscv_has_atomic()) {
        uint32_t result;
        asm volatile ("amoadd.w %0, %2, (%1)" : "=r"(result) : "r"(ptr), "r"(value) : "memory");
        return result;
    } else {
        uint32_t old = *ptr;
        *ptr = old + value;
        return old;
    }
}

static inline uint64_t riscv_atomic_add64(volatile uint64_t* ptr, uint64_t value) {
    if (riscv_has_atomic()) {
        uint64_t result;
        asm volatile ("amoadd.d %0, %2, (%1)" : "=r"(result) : "r"(ptr), "r"(value) : "memory");
        return result;
    } else {
        uint64_t old = *ptr;
        *ptr = old + value;
        return old;
    }
}

static inline int riscv_compare_and_swap32(volatile uint32_t* ptr, uint32_t expected, uint32_t desired) {
    if (riscv_has_atomic()) {
        uint32_t current;
        int success;
        asm volatile (
            "1: lr.w %0, (%2)\n"
            "   bne %0, %3, 2f\n"
            "   sc.w %1, %4, (%2)\n"
            "   bnez %1, 1b\n"
            "   li %1, 1\n"
            "   j 3f\n"
            "2: li %1, 0\n"
            "3:"
            : "=&r"(current), "=&r"(success)
            : "r"(ptr), "r"(expected), "r"(desired)
            : "memory"
        );
        return success;
    } else {
        if (*ptr == expected) {
            *ptr = desired;
            return 1;
        }
        return 0;
    }
}

static inline int riscv_compare_and_swap64(volatile uint64_t* ptr, uint64_t expected, uint64_t desired) {
    if (riscv_has_atomic()) {
        uint64_t current;
        int success;
        asm volatile (
            "1: lr.d %0, (%2)\n"
            "   bne %0, %3, 2f\n"
            "   sc.d %1, %4, (%2)\n"
            "   bnez %1, 1b\n"
            "   li %1, 1\n"
            "   j 3f\n"
            "2: li %1, 0\n"
            "3:"
            : "=&r"(current), "=&r"(success)
            : "r"(ptr), "r"(expected), "r"(desired)
            : "memory"
        );
        return success;
    } else {
        if (*ptr == expected) {
            *ptr = desired;
            return 1;
        }
        return 0;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* XMRIG_RISCV_MEMORY_H */