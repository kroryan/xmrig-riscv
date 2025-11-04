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

#ifndef XMRIG_BASICCPUINFO_RISCV_H
#define XMRIG_BASICCPUINFO_RISCV_H

#include "backend/cpu/interfaces/ICpuInfo.h"

#include <bitset>
#include <string>
#include <vector>

namespace xmrig {

class BasicCpuInfo : public ICpuInfo
{
public:
    BasicCpuInfo();

protected:
    Arch arch() const override                                                       { return ARCH_UNKNOWN; }
    Assembly::Id assembly() const override;
    bool has(Flag feature) const override                                            { return m_flags.test(feature); }
    bool hasAES() const override;
    bool hasVAES() const override                                                    { return false; }
    bool hasAVX() const override                                                     { return false; }
    bool hasAVX2() const override                                                    { return false; }
    bool hasBMI2() const override                                                    { return false; }
    bool hasCatL3() const override                                                   { return false; }
    bool hasOneGbPages() const override;
    bool hasXOP() const override                                                     { return false; }
    bool isVM() const override                                                       { return m_flags.test(FLAG_VM); }
    bool jccErratum() const override                                                 { return false; }
    const char *backend() const override;
    const char *brand() const override                                               { return m_brand; }
    const std::vector<int32_t> &units() const override                               { return m_units; }
    CpuThreads threads(const Algorithm &algorithm, uint32_t limit) const override;
    MsrMod msrMod() const override                                                   { return MSR_MOD_NONE; }
    rapidjson::Value toJSON(rapidjson::Document &doc) const override;
    size_t cores() const override                                                    { return 0; }
    size_t L2() const override                                                       { return 0; }
    size_t L3() const override                                                       { return 0; }
    size_t nodes() const override                                                    { return 0; }
    size_t packages() const override                                                 { return 1; }
    size_t threads() const override                                                  { return m_threads; }
    Vendor vendor() const override                                                   { return m_vendor; }
    uint32_t model() const override                                                  { return 0; }

#   ifdef XMRIG_FEATURE_HWLOC
    bool membind(hwloc_const_bitmap_t nodeset) override                              { return false; }
    const std::vector<uint32_t> &nodeset() const override                           { return m_nodeset; }
    hwloc_topology_t topology() const override                                      { return nullptr; }
#   endif

    // RISC-V specific extensions
    bool hasZbb() const                                                              { return m_hasZbb; }
    bool hasZbc() const                                                              { return m_hasZbc; }
    bool hasZbs() const                                                              { return m_hasZbs; }
    bool hasRvv() const                                                              { return m_hasRvv; }

private:
    void detectRiscvExtensions();
    void parseCpuInfo();
    void parseIsaString(const char* isa);

    Arch m_arch             = ARCH_UNKNOWN;
    bool m_jccErratum       = false;
    char m_brand[64 + 6]{};
    size_t m_threads        = 0;
    std::vector<int32_t> m_units;
    Vendor m_vendor         = VENDOR_UNKNOWN;

private:
#   ifndef XMRIG_ARM
    uint32_t m_procInfo     = 0;
    uint32_t m_family       = 0;
    uint32_t m_model        = 0;
    uint32_t m_stepping     = 0;
#   endif

    Assembly m_assembly     = Assembly::NONE;
    MsrMod m_msrMod         = MSR_MOD_NONE;
    std::bitset<FLAG_MAX> m_flags;
    
    // RISC-V specific extension flags
    bool m_hasZbb;
    bool m_hasZbc;
    bool m_hasZbs;
    bool m_hasRvv;

#   ifdef XMRIG_FEATURE_HWLOC
    std::vector<uint32_t> m_nodeset;
#   endif
};

} // namespace xmrig

#endif // XMRIG_BASICCPUINFO_RISCV_H