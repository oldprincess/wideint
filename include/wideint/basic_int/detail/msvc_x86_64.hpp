#ifndef WIDEINT_BASIC_INT_DETAIL_MSVC_X86_64_HPP
#define WIDEINT_BASIC_INT_DETAIL_MSVC_X86_64_HPP

#pragma once

#include <cstdint>

#include <intrin.h>

#include <wideint/basic_int/detail/common.hpp>
#include <wideint/basic_int/detail/fallback.hpp>

namespace wideint::detail {

WIDEINT_HD inline add_result msvc_x86_64_add_carry_u64(std::uint64_t a, std::uint64_t b,
                                                       std::uint64_t carry_in) {
    unsigned __int64 sum = 0;
    const unsigned char carry_out =
        _addcarry_u64(static_cast<unsigned char>(carry_in), static_cast<unsigned __int64>(a),
                      static_cast<unsigned __int64>(b), &sum);
    return {static_cast<std::uint64_t>(sum), carry_out};
}

WIDEINT_HD inline sub_result msvc_x86_64_sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                                         std::uint64_t borrow_in) {
    unsigned __int64 diff = 0;
    const unsigned char borrow_out =
        _subborrow_u64(static_cast<unsigned char>(borrow_in), static_cast<unsigned __int64>(a),
                       static_cast<unsigned __int64>(b), &diff);
    return {static_cast<std::uint64_t>(diff), borrow_out};
}

WIDEINT_HD inline mul_result msvc_x86_64_mul_wide_u64(std::uint64_t a, std::uint64_t b) {
    unsigned __int64 hi = 0;
#if defined(__BMI2__)
    const unsigned __int64 lo =
        _mulx_u64(static_cast<unsigned __int64>(a), static_cast<unsigned __int64>(b), &hi);
#else
    const unsigned __int64 lo =
        _umul128(static_cast<unsigned __int64>(a), static_cast<unsigned __int64>(b), &hi);
#endif
    return {static_cast<std::uint64_t>(lo), static_cast<std::uint64_t>(hi)};
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_MSVC_X86_64_HPP
