#ifndef WIDEINT_BASIC_INT_DETAIL_FALLBACK_HPP
#define WIDEINT_BASIC_INT_DETAIL_FALLBACK_HPP

#pragma once

#include <cstdint>

#include <wideint/basic_int/detail/common.hpp>

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

WIDEINT_HD inline add_result fallback_add_carry_u64(std::uint64_t a, std::uint64_t b,
                                                    std::uint64_t carry_in) {
    const std::uint64_t sum = a + b;
    const std::uint64_t carry0 = sum < a ? 1u : 0u;
    const std::uint64_t sum2 = sum + carry_in;
    const std::uint64_t carry1 = sum2 < sum ? 1u : 0u;
    return {sum2, carry0 | carry1};
}

WIDEINT_HD inline sub_result fallback_sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                                      std::uint64_t borrow_in) {
    const std::uint64_t diff = a - b;
    const std::uint64_t borrow0 = a < b ? 1u : 0u;
    const std::uint64_t diff2 = diff - borrow_in;
    const std::uint64_t borrow1 = diff < borrow_in ? 1u : 0u;
    return {diff2, borrow0 | borrow1};
}

WIDEINT_HD inline mul_result fallback_mul_wide_u64(std::uint64_t a, std::uint64_t b) {
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
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_FALLBACK_HPP
