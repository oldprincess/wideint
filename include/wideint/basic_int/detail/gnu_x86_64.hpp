#ifndef WIDEINT_BASIC_INT_DETAIL_GNU_X86_64_HPP
#define WIDEINT_BASIC_INT_DETAIL_GNU_X86_64_HPP

#pragma once

#include <cstdint>

#include <immintrin.h>

#include <wideint/basic_int/detail/common.hpp>
#include <wideint/basic_int/detail/fallback.hpp>

namespace wideint::detail {

WIDEINT_HD inline add_result gnu_x86_64_add_carry_u64(std::uint64_t a, std::uint64_t b,
                                                      std::uint64_t carry_in) {
    unsigned long long sum = 0;
    const unsigned char carry_out =
        _addcarry_u64(static_cast<unsigned char>(carry_in), a, b, &sum);
    return {static_cast<std::uint64_t>(sum), static_cast<std::uint64_t>(carry_out)};
}

WIDEINT_HD inline sub_result gnu_x86_64_sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                                        std::uint64_t borrow_in) {
    unsigned long long diff = 0;
    const unsigned char borrow_out =
        _subborrow_u64(static_cast<unsigned char>(borrow_in), a, b, &diff);
    return {static_cast<std::uint64_t>(diff), static_cast<std::uint64_t>(borrow_out)};
}

WIDEINT_HD inline mul_result gnu_x86_64_mul_wide_u64(std::uint64_t a, std::uint64_t b) {
#if defined(__BMI2__)
    unsigned long long hi = 0;
    const unsigned long long lo = _mulx_u64(a, b, &hi);
    return {static_cast<std::uint64_t>(lo), static_cast<std::uint64_t>(hi)};
#elif defined(__SIZEOF_INT128__)
    const unsigned __int128 product =
        static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b);
    return {static_cast<std::uint64_t>(product),
            static_cast<std::uint64_t>(static_cast<unsigned __int128>(product >> 64))};
#else
    return fallback_mul_wide_u64(a, b);
#endif
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_GNU_X86_64_HPP
