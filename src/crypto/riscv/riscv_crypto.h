/*
 * XMRig RISC-V crypto optimizations
 * Copyright (c) 2024 XMRig developers
 * 
 * This file implements RISC-V specific crypto operations including
 * Zbb/Zbk bit manipulation and future Zkn* crypto extensions support.
 */

#ifndef XMRIG_RISCV_CRYPTO_H
#define XMRIG_RISCV_CRYPTO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// RISC-V bit manipulation extension detection
static inline int riscv_has_zbb() {
#ifdef XMRIG_RISCV_OPTIMIZED
    return 1; // Assume Zbb is available if we're optimizing for RISC-V
#else
    return 0;
#endif
}

static inline int riscv_has_zbkb() {
#ifdef XMRIG_RISCV_OPTIMIZED
    return 1; // Assume Zbkb is available if we're optimizing for RISC-V
#else
    return 0;
#endif
}

// RISC-V optimized rotate right (using Zbb if available)
static inline uint32_t riscv_rotr32(uint32_t value, int bits) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        uint32_t result;
        asm ("ror %0, %1, %2" : "=r"(result) : "r"(value), "r"(bits));
        return result;
#else
        return (value >> bits) | (value << (32 - bits));
#endif
    } else {
        return (value >> bits) | (value << (32 - bits));
    }
}

static inline uint64_t riscv_rotr64(uint64_t value, int bits) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        uint64_t result;
        asm ("ror %0, %1, %2" : "=r"(result) : "r"(value), "r"(bits));
        return result;
#else
        return (value >> bits) | (value << (64 - bits));
#endif
    } else {
        return (value >> bits) | (value << (64 - bits));
    }
}

// RISC-V optimized rotate left
static inline uint32_t riscv_rotl32(uint32_t value, int bits) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        uint32_t result;
        asm ("rol %0, %1, %2" : "=r"(result) : "r"(value), "r"(bits));
        return result;
#else
        return (value << bits) | (value >> (32 - bits));
#endif
    } else {
        return (value << bits) | (value >> (32 - bits));
    }
}

static inline uint64_t riscv_rotl64(uint64_t value, int bits) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        uint64_t result;
        asm ("rol %0, %1, %2" : "=r"(result) : "r"(value), "r"(bits));
        return result;
#else
        return (value << bits) | (value >> (64 - bits));
#endif
    } else {
        return (value << bits) | (value >> (64 - bits));
    }
}

// Population count (using Zbb if available)
static inline int riscv_popcount32(uint32_t value) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        int result;
        asm ("cpop %0, %1" : "=r"(result) : "r"(value));
        return result;
#else
        return __builtin_popcount(value);
#endif
    } else {
        return __builtin_popcount(value);
    }
}

static inline int riscv_popcount64(uint64_t value) {
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        int result;
        asm ("cpop %0, %1" : "=r"(result) : "r"(value));
        return result;
#else
        return __builtin_popcountl(value);
#endif
    } else {
        return __builtin_popcountl(value);
    }
}

// Count trailing zeros (using Zbb if available)
static inline int riscv_ctz32(uint32_t value) {
    if (value == 0) return 32;
    
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        int result;
        asm ("ctz %0, %1" : "=r"(result) : "r"(value));
        return result;
#else
        return __builtin_ctz(value);
#endif
    } else {
        return __builtin_ctz(value);
    }
}

static inline int riscv_ctz64(uint64_t value) {
    if (value == 0) return 64;
    
    if (riscv_has_zbb()) {
#if defined(__riscv_zbb) || defined(XMRIG_RISCV_OPTIMIZED)
        int result;
        asm ("ctz %0, %1" : "=r"(result) : "r"(value));
        return result;
#else
        return __builtin_ctzl(value);
#endif
    } else {
        return __builtin_ctzl(value);
    }
}

// Future crypto extension stubs (for Zkn*, Zkd, etc.)
// These will be implemented when RISC-V crypto extensions become available

// AES operations (fallback to software implementations for now)
static inline void riscv_aes_encrypt_block_soft(const uint8_t* plaintext, uint8_t* ciphertext, const uint8_t* key) {
    // This should call the soft AES implementation
    // For now, this is a placeholder - actual implementation will use soft_aes.h
    (void)plaintext;
    (void)ciphertext;
    (void)key;
    // TODO: Implement soft AES fallback
}

static inline void riscv_aes_decrypt_block_soft(const uint8_t* ciphertext, uint8_t* plaintext, const uint8_t* key) {
    // This should call the soft AES implementation  
    // For now, this is a placeholder - actual implementation will use soft_aes.h
    (void)ciphertext;
    (void)plaintext;
    (void)key;
    // TODO: Implement soft AES fallback
}

// SHA operations (future Zknh support)
static inline void riscv_sha256_compress_soft(uint32_t state[8], const uint8_t block[64]) {
    // Software fallback for SHA-256
    (void)state;
    (void)block;
    // TODO: Implement optimized software SHA-256
}

#ifdef __cplusplus
}
#endif

#endif /* XMRIG_RISCV_CRYPTO_H */