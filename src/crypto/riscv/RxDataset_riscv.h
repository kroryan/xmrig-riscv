/*
 * XMRig RISC-V RandomX dataset optimization
 * Copyright (c) 2024 XMRig developers
 * 
 * This file implements RISC-V specific optimizations for RandomX dataset
 * initialization including core affinity, adaptive threads, and memory barriers.
 */

#ifndef XMRIG_RXDATASET_RISCV_H
#define XMRIG_RXDATASET_RISCV_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// RISC-V specific dataset initialization optimizations
typedef struct {
    int num_cores;
    int cache_line_size;
    int l1_cache_size;
    int l2_cache_size;
    int has_vector;
    int has_atomic;
} riscv_cpu_info_t;

// Get RISC-V CPU information
static inline void riscv_get_cpu_info(riscv_cpu_info_t* info) {
    // Default values for typical RISC-V systems
    info->num_cores = 8;  // Orange Pi RV2 default
    info->cache_line_size = 64;
    info->l1_cache_size = 32 * 1024;   // 32KB L1
    info->l2_cache_size = 512 * 1024;  // 512KB L2
    
#ifdef XMRIG_RVV_ENABLED
    info->has_vector = 1;
#else
    info->has_vector = 0;
#endif

#ifdef XMRIG_RISCV_OPTIMIZED
    info->has_atomic = 1;
#else
    info->has_atomic = 0;
#endif

    // TODO: Add runtime detection of actual CPU capabilities
}

// Set CPU affinity for RISC-V cores
static inline int riscv_set_cpu_affinity(int core_id) {
#ifdef __linux__
    // Use Linux CPU affinity on RISC-V systems
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    
    return sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
#else
    (void)core_id;
    return 0; // No affinity support on non-Linux systems
#endif
}

// Adaptive thread count based on dataset size and CPU capabilities
static inline int riscv_calculate_optimal_threads(size_t dataset_size, const riscv_cpu_info_t* cpu_info) {
    // Base thread count on number of cores
    int threads = cpu_info->num_cores;
    
    // Adjust based on dataset size and cache
    if (dataset_size > 1024 * 1024 * 1024) { // > 1GB
        // Large dataset benefits from more threads
        threads = cpu_info->num_cores;
    } else if (dataset_size > 256 * 1024 * 1024) { // > 256MB
        // Medium dataset - use fewer threads to avoid cache thrashing
        threads = cpu_info->num_cores * 3 / 4;
    } else {
        // Small dataset - use even fewer threads
        threads = cpu_info->num_cores / 2;
    }
    
    // Ensure at least one thread
    return threads > 0 ? threads : 1;
}

// Cache-aligned memory copy for dataset initialization
static inline void riscv_dataset_memcpy_aligned(void* dest, const void* src, size_t size, int cache_line_size) {
    const char* s = (const char*)src;
    char* d = (char*)dest;
    
    // Ensure alignment
    uintptr_t dest_align = (uintptr_t)dest & (cache_line_size - 1);
    uintptr_t src_align = (uintptr_t)src & (cache_line_size - 1);
    
    if (dest_align == 0 && src_align == 0 && size >= cache_line_size * 4) {
        // Both aligned, use optimized copy
        size_t aligned_size = size & ~(cache_line_size - 1);
        size_t remaining = size & (cache_line_size - 1);
        
        // Copy cache line aligned blocks
        for (size_t i = 0; i < aligned_size; i += cache_line_size) {
            // Prefetch next cache line
            if (i + cache_line_size * 2 < aligned_size) {
                __builtin_prefetch(s + i + cache_line_size * 2, 0, 3);
                __builtin_prefetch(d + i + cache_line_size * 2, 1, 3);
            }
            
            // Copy current cache line
            for (int j = 0; j < cache_line_size; j += 8) {
                *(uint64_t*)(d + i + j) = *(const uint64_t*)(s + i + j);
            }
        }
        
        // Copy remaining bytes
        for (size_t i = 0; i < remaining; i++) {
            d[aligned_size + i] = s[aligned_size + i];
        }
    } else {
        // Fallback to standard memcpy
        for (size_t i = 0; i < size; i++) {
            d[i] = s[i];
        }
    }
}

// Memory barrier for dataset consistency
static inline void riscv_dataset_memory_barrier() {
    asm volatile ("fence rw, rw" : : : "memory");
}

// RISC-V specific dataset initialization function
static inline void riscv_init_dataset_worker(void* dataset, size_t start_offset, size_t size, 
                                            const void* cache, size_t cache_size, int thread_id) {
    riscv_cpu_info_t cpu_info;
    riscv_get_cpu_info(&cpu_info);
    
    // Set CPU affinity for this thread
    if (thread_id >= 0 && thread_id < cpu_info.num_cores) {
        riscv_set_cpu_affinity(thread_id);
    }
    
    // Ensure memory ordering
    riscv_dataset_memory_barrier();
    
    // Calculate work chunk size based on cache size
    size_t chunk_size = cpu_info.l2_cache_size / 4; // Use 1/4 of L2 cache
    if (chunk_size < 4096) chunk_size = 4096;
    if (chunk_size > 65536) chunk_size = 65536;
    
    char* dst = (char*)dataset + start_offset;
    const char* src = (const char*)cache;
    
    // Process in cache-friendly chunks
    for (size_t offset = 0; offset < size; offset += chunk_size) {
        size_t current_chunk = (offset + chunk_size <= size) ? chunk_size : (size - offset);
        
        // Prefetch source data
        for (size_t i = 0; i < current_chunk && i < cache_size; i += cpu_info.cache_line_size) {
            __builtin_prefetch(src + (offset % cache_size) + i, 0, 3);
        }
        
        // Copy chunk with alignment optimization
        riscv_dataset_memcpy_aligned(dst + offset, 
                                   src + (offset % cache_size), 
                                   current_chunk, 
                                   cpu_info.cache_line_size);
        
        // Memory barrier every few chunks to ensure consistency
        if ((offset / chunk_size) % 16 == 15) {
            riscv_dataset_memory_barrier();
        }
    }
    
    // Final memory barrier
    riscv_dataset_memory_barrier();
}

// Calculate optimal chunk size for dataset initialization
static inline size_t riscv_calculate_chunk_size(size_t total_size, int num_threads, const riscv_cpu_info_t* cpu_info) {
    // Base chunk size on L2 cache size
    size_t base_chunk = cpu_info->l2_cache_size;
    
    // Adjust for number of threads
    size_t chunk_size = base_chunk / num_threads;
    
    // Ensure minimum chunk size
    if (chunk_size < 1024 * 1024) {  // Minimum 1MB
        chunk_size = 1024 * 1024;
    }
    
    // Ensure maximum chunk size
    if (chunk_size > 64 * 1024 * 1024) {  // Maximum 64MB
        chunk_size = 64 * 1024 * 1024;
    }
    
    // Align to cache line boundary
    chunk_size = (chunk_size + cpu_info->cache_line_size - 1) & ~(cpu_info->cache_line_size - 1);
    
    return chunk_size;
}

#ifdef __cplusplus
}
#endif

#endif /* XMRIG_RXDATASET_RISCV_H */