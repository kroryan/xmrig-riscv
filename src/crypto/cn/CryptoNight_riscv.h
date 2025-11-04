/* XMRig
 * Copyright 2010      Jeff Garzik  <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler       <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones  <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466     <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee    <jayddee246@gmail.com>
 * Copyright 2016      Imran Yusuff <https://github.com/imranyusuff>
 * Copyright 2017-2019 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018      Lee Clagett <https://github.com/vtnerd>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 * Copyright 2024      RISC-V Port <https://github.com/kroryan/xmrig-riscv>
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

#ifndef XMRIG_CRYPTONIGHT_RISCV_H
#define XMRIG_CRYPTONIGHT_RISCV_H

// RISC-V: CryptoNight algorithms are disabled for this architecture
// This file provides minimal stubs to prevent compilation errors
// RandomX is the primary and only supported algorithm for RISC-V

#include "base/crypto/keccak.h"
#include "crypto/cn/CnAlgo.h"
#include "crypto/cn/CryptoNight.h"

extern "C"
{
#include "crypto/cn/c_groestl.h"
#include "crypto/cn/c_blake256.h"
#include "crypto/cn/c_jh.h"
#include "crypto/cn/c_skein.h"
}

namespace xmrig {

// Hash function implementations for CryptoNight (stubs only)
static inline void do_blake_hash(const uint8_t *input, size_t len, uint8_t *output) {
    blake256_hash(output, input, len);
}

static inline void do_groestl_hash(const uint8_t *input, size_t len, uint8_t *output) {
    groestl(input, len * 8, output);
}

static inline void do_jh_hash(const uint8_t *input, size_t len, uint8_t *output) {
    jh_hash(32 * 8, input, 8 * len, output);
}

static inline void do_skein_hash(const uint8_t *input, size_t len, uint8_t *output) {
    xmr_skein(input, output);
}

void (* const extra_hashes[4])(const uint8_t *, size_t, uint8_t *) = {
    do_blake_hash, do_groestl_hash, do_jh_hash, do_skein_hash
};

// Minimal CryptoNight implementations (disabled for RISC-V)
template<Algorithm::Id ALGO, bool SOFT_AES>
inline void cryptonight_single_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t height)
{
    // CryptoNight algorithms are disabled for RISC-V architecture
    // Use RandomX instead
}

template<Algorithm::Id ALGO, bool SOFT_AES>
inline void cryptonight_double_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t height)
{
    // CryptoNight algorithms are disabled for RISC-V architecture
}

template<Algorithm::Id ALGO, bool SOFT_AES>
inline void cryptonight_triple_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t height)
{
    // CryptoNight algorithms are disabled for RISC-V architecture
}

template<Algorithm::Id ALGO, bool SOFT_AES>
inline void cryptonight_quad_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t height)
{
    // CryptoNight algorithms are disabled for RISC-V architecture
}

template<Algorithm::Id ALGO, bool SOFT_AES>
inline void cryptonight_penta_hash(const uint8_t *__restrict__ input, size_t size, uint8_t *__restrict__ output, cryptonight_ctx **__restrict__ ctx, uint64_t height)
{
    // CryptoNight algorithms are disabled for RISC-V architecture
}

} /* namespace xmrig */

#endif /* XMRIG_CRYPTONIGHT_RISCV_H */