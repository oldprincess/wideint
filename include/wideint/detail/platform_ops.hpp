#ifndef WIDEINT_DETAIL_PLATFORM_OPS_HPP
#define WIDEINT_DETAIL_PLATFORM_OPS_HPP

#pragma once

#include <cstdint>

#include "common.hpp"

#if !defined(__CUDACC__)
#include <immintrin.h>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#define _mulx_u64(a, b, c) _mulx_u64(a, b, (long long unsigned int *)c)
#define _addcarry_u64(a, b, c, d) _addcarry_u64(a, b, c, (long long unsigned int *)d)
#define _subborrow_u64(a, b, c, d) _subborrow_u64(a, b, c, (long long unsigned int *)d)
#endif
#endif

namespace wideint::detail {

struct add_result {
    std::uint64_t value;
    std::uint64_t carry;
};

struct sub_result {
    std::uint64_t value;
    std::uint64_t borrow;
};

struct mul_result {
    std::uint64_t lo;
    std::uint64_t hi;
};

#if defined(__CUDA_ARCH__)
WIDEINT_HD inline add_result cuda_addc_u64(std::uint64_t a, std::uint64_t b) {
    unsigned long long sum = 0;
    unsigned long long carry = 0;
    asm volatile("add.cc.u64 %0, %2, %3;\n\t"
                 "addc.u64 %1, 0, 0;"
                 : "=l"(sum), "=l"(carry)
                 : "l"(a), "l"(b));
    return {sum, carry};
}

WIDEINT_HD inline sub_result cuda_subc_u64(std::uint64_t a, std::uint64_t b) {
    unsigned long long diff = 0;
    asm volatile("sub.u64 %0, %1, %2;" : "=l"(diff) : "l"(a), "l"(b));
    return {diff, a < b ? 1u : 0u};
}
#endif

WIDEINT_HD inline add_result add_carry_u64(std::uint64_t a, std::uint64_t b,
                                           std::uint64_t carry_in) {
#if defined(__CUDA_ARCH__)
    const auto s0 = cuda_addc_u64(a, b);
    const auto s1 = cuda_addc_u64(s0.value, carry_in);
    return {s1.value, s0.carry | s1.carry};
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    unsigned __int64 sum = 0;
    const unsigned char carry_out =
        _addcarry_u64(static_cast<unsigned char>(carry_in), static_cast<unsigned __int64>(a),
                      static_cast<unsigned __int64>(b), &sum);
    return {sum, carry_out};
#else
    const std::uint64_t sum = a + b;
    const std::uint64_t carry0 = sum < a ? 1u : 0u;
    const std::uint64_t sum2 = sum + carry_in;
    const std::uint64_t carry1 = sum2 < sum ? 1u : 0u;
    return {sum2, carry0 | carry1};
#endif
}

WIDEINT_HD inline sub_result sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                            std::uint64_t borrow_in) {
#if defined(__CUDA_ARCH__)
    const auto d0 = cuda_subc_u64(a, b);
    const auto d1 = cuda_subc_u64(d0.value, borrow_in);
    return {d1.value, d0.borrow | d1.borrow};
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    unsigned __int64 diff = 0;
    const unsigned char borrow_out =
        _subborrow_u64(static_cast<unsigned char>(borrow_in), static_cast<unsigned __int64>(a),
                       static_cast<unsigned __int64>(b), &diff);
    return {diff, borrow_out};
#else
    const std::uint64_t diff = a - b;
    const std::uint64_t borrow0 = a < b ? 1u : 0u;
    const std::uint64_t diff2 = diff - borrow_in;
    const std::uint64_t borrow1 = diff < borrow_in ? 1u : 0u;
    return {diff2, borrow0 | borrow1};
#endif
}

WIDEINT_HD inline mul_result mul_wide_u64(std::uint64_t a, std::uint64_t b) {
#if defined(__CUDA_ARCH__)
    return {a * b, __umul64hi(a, b)};
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    unsigned __int64 hi = 0;
#if defined(__BMI2__)
    const unsigned __int64 lo =
        _mulx_u64(static_cast<unsigned __int64>(a), static_cast<unsigned __int64>(b), &hi);
#else
    const unsigned __int64 lo =
        _umul128(static_cast<unsigned __int64>(a), static_cast<unsigned __int64>(b), &hi);
#endif
    return {lo, hi};
#elif defined(__SIZEOF_INT128__)
    const unsigned __int128 product =
        static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b);
    return {static_cast<std::uint64_t>(product), static_cast<std::uint64_t>(product >> 64)};
#else
    const std::uint64_t a_lo = static_cast<std::uint32_t>(a);
    const std::uint64_t a_hi = a >> 32;
    const std::uint64_t b_lo = static_cast<std::uint32_t>(b);
    const std::uint64_t b_hi = b >> 32;

    const std::uint64_t p0 = a_lo * b_lo;
    const std::uint64_t p1 = a_lo * b_hi;
    const std::uint64_t p2 = a_hi * b_lo;
    const std::uint64_t p3 = a_hi * b_hi;

    const std::uint64_t mid =
        (p0 >> 32) + static_cast<std::uint32_t>(p1) + static_cast<std::uint32_t>(p2);
    const std::uint64_t lo = (p0 & 0xffffffffull) | (mid << 32);
    const std::uint64_t hi = p3 + (p1 >> 32) + (p2 >> 32) + (mid >> 32);
    return {lo, hi};
#endif
}

} // namespace wideint::detail

#endif // WIDEINT_DETAIL_PLATFORM_OPS_HPP
