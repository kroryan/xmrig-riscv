/* XMRig
 * Copyright (c) 2024 XMRig developers
 * Copyright (c) 2018-2023 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2023 XMRig       <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "backend/cpu/platform/BasicCpuInfo_riscv.h"
#include "3rdparty/rapidjson/document.h"
#include "crypto/common/Assembly.h"

#include <cstring>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <bitset>

#ifdef __linux__
#   include <unistd.h>
#   include <sys/sysinfo.h>
#   include <sched.h>
#endif

xmrig::BasicCpuInfo::BasicCpuInfo()
{
    strcpy(m_brand, "RISC-V Processor");
    m_vendor = VENDOR_UNKNOWN;
    m_arch = ARCH_UNKNOWN;
    m_threads = std::thread::hardware_concurrency();
    m_assembly = Assembly::NONE;
    
    // RISC-V specific feature detection
    m_flags[FLAG_AES] = false;        // Hardware AES not available yet
    m_flags[FLAG_VAES] = false;       // VAES not applicable to RISC-V
    m_flags[FLAG_AVX] = false;        // x86-specific
    m_flags[FLAG_AVX2] = false;       // x86-specific  
    m_flags[FLAG_AVX512F] = false;    // x86-specific
    m_flags[FLAG_BMI2] = false;       // x86-specific
    m_flags[FLAG_OSXSAVE] = false;    // x86-specific
    m_flags[FLAG_PDPE1GB] = true;     // Assume 1GB pages support
    m_flags[FLAG_SSE2] = false;       // x86-specific
    m_flags[FLAG_SSSE3] = false;      // x86-specific
    m_flags[FLAG_SSE41] = false;      // x86-specific
    m_flags[FLAG_XOP] = false;        // x86-specific
    m_flags[FLAG_POPCNT] = true;      // Available via Zbb extension
    m_flags[FLAG_CAT_L3] = false;     // Intel-specific
    m_flags[FLAG_VM] = false;         // Not running in VM by default

    // Detect RISC-V extensions
    detectRiscvExtensions();
    
    // Parse /proc/cpuinfo on Linux for more accurate information
    parseCpuInfo();
}

void xmrig::BasicCpuInfo::detectRiscvExtensions()
{
#ifdef __riscv
    // Check for standard extensions
    m_hasZbb = false;
    m_hasZbc = false;
    m_hasZbs = false;
    m_hasRvv = false;
    
#   ifdef __riscv_zbb
    m_hasZbb = true;
    m_flags[FLAG_POPCNT] = true;
#   endif

#   ifdef __riscv_zbc
    m_hasZbc = true;
#   endif

#   ifdef __riscv_zbs
    m_hasZbs = true;
#   endif

#   ifdef __riscv_vector
    m_hasRvv = true;
#   endif

    // Additional compile-time checks
#   ifdef XMRIG_RISCV_OPTIMIZED
    m_hasZbb = true;
    m_hasZbc = true; 
    m_hasZbs = true;
#   endif

#   ifdef XMRIG_RVV_ENABLED
    m_hasRvv = true;
#   endif

#else
    m_hasZbb = false;
    m_hasZbc = false;
    m_hasZbs = false;
    m_hasRvv = false;
#endif
}

void xmrig::BasicCpuInfo::parseCpuInfo()
{
#ifdef __linux__
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processor", 9) == 0) {
            // Count processors
        }
        else if (strncmp(line, "model name", 10) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                char* brand = colon + 2; // Skip ": "
                // Remove newline
                char* newline = strchr(brand, '\n');
                if (newline) *newline = '\0';
                strncpy(m_brand, brand, sizeof(m_brand) - 1);
                m_brand[sizeof(m_brand) - 1] = '\0';
            }
        }
        else if (strncmp(line, "isa", 3) == 0) {
            char* colon = strchr(line, ':');
            if (colon) {
                char* isa = colon + 2; // Skip ": "
                parseIsaString(isa);
            }
        }
    }
    
    fclose(fp);
    
    // Ensure reasonable defaults
    if (m_threads == 0) m_threads = 1;
#endif
}

void xmrig::BasicCpuInfo::parseIsaString(const char* isa)
{
    if (!isa) return;
    
    // Parse RISC-V ISA string to detect extensions
    if (strstr(isa, "zbb")) {
        m_hasZbb = true;
        m_flags[FLAG_POPCNT] = true;
    }
    
    if (strstr(isa, "zbc")) {
        m_hasZbc = true;
    }
    
    if (strstr(isa, "zbs")) {
        m_hasZbs = true;
    }
    
    if (strstr(isa, "v")) {
        m_hasRvv = true;
    }
    
    // Check for future crypto extensions
    if (strstr(isa, "zkn")) {
        // Future: NIST cryptography support
    }
    
    if (strstr(isa, "zkd")) {
        // Future: SHA support  
    }
}

xmrig::Assembly::Id xmrig::BasicCpuInfo::assembly() const
{
    // RISC-V doesn't have x86-style assembly optimizations
    // Return NONE to use C++ implementations
    return Assembly::NONE;
}

bool xmrig::BasicCpuInfo::hasAES() const
{
    // Hardware AES not available on current RISC-V implementations
    return false;
}

bool xmrig::BasicCpuInfo::hasOneGbPages() const
{
    // Check for 1GB page support
#ifdef __linux__
    FILE* fp = fopen("/proc/meminfo", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "HugePages_Total:", 16) == 0) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }
#endif
    return true; // Assume support by default
}

xmrig::CpuThreads xmrig::BasicCpuInfo::threads(const Algorithm &algorithm, uint32_t limit) const
{
    // RISC-V specific thread configuration
    const size_t count = std::min<size_t>(limit, m_threads);
    
    if (algorithm.family() == Algorithm::RANDOM_X) {
        // RandomX works well with 1 thread per physical core
        return CpuThreads(count);
    }
    
    if (algorithm.family() == Algorithm::ARGON2) {
        // Argon2 can use more threads
        return CpuThreads(count);
    }
    
    if (algorithm.family() == Algorithm::GHOSTRIDER) {
        // GhostRider benefits from multiple threads
        return CpuThreads(count);
    }
    
    // Default: use all available threads
    return CpuThreads(count);
}

rapidjson::Value xmrig::BasicCpuInfo::toJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    
    Value out(kObjectType);
    auto &allocator = doc.GetAllocator();
    
    out.AddMember("brand", StringRef(m_brand), allocator);
    out.AddMember("family", 0, allocator);
    out.AddMember("model", 0, allocator);
    out.AddMember("stepping", 0, allocator);
    out.AddMember("proc_info", 0, allocator);
    out.AddMember("aes", hasAES(), allocator);
    out.AddMember("avx2", false, allocator);
    out.AddMember("x64", ICpuInfo::is64bit(), allocator);
    out.AddMember("64_bit", ICpuInfo::is64bit(), allocator);
    out.AddMember("l2", static_cast<uint64_t>(0), allocator);
    out.AddMember("l3", static_cast<uint64_t>(0), allocator);
    out.AddMember("cores", static_cast<uint64_t>(0), allocator);
    out.AddMember("threads", static_cast<uint64_t>(m_threads), allocator);
    out.AddMember("packages", static_cast<uint64_t>(1), allocator);
    out.AddMember("nodes", static_cast<uint64_t>(0), allocator);
    out.AddMember("backend", StringRef(backend()), allocator);
    out.AddMember("msr", "none", allocator);
    
    // RISC-V: avoid linking x86 ASM utilities; report "none"
    out.AddMember("assembly", StringRef("none"), allocator);
    out.AddMember("arch", "riscv64", allocator);
    
    // RISC-V specific extensions
    Value extensions(kObjectType);
    extensions.AddMember("zbb", m_hasZbb, allocator);
    extensions.AddMember("zbc", m_hasZbc, allocator);
    extensions.AddMember("zbs", m_hasZbs, allocator);
    extensions.AddMember("rvv", m_hasRvv, allocator);
    out.AddMember("riscv_extensions", extensions, allocator);
    
    return out;
}

const char *xmrig::BasicCpuInfo::backend() const
{
    return "basic";
}