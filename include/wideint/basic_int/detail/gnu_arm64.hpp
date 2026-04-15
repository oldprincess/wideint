#ifndef WIDEINT_BASIC_INT_DETAIL_GNU_ARM64_HPP
#define WIDEINT_BASIC_INT_DETAIL_GNU_ARM64_HPP

#pragma once

#include <cstdint>

#include <wideint/basic_int/detail/common.hpp>
#include <wideint/basic_int/detail/fallback.hpp>

namespace wideint::detail {

WIDEINT_HD inline add_result gnu_arm64_add_carry_u64(std::uint64_t a, std::uint64_t b,
                                                     std::uint64_t carry_in) {
    return fallback_add_carry_u64(a, b, carry_in);
}

WIDEINT_HD inline sub_result gnu_arm64_sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                                      std::uint64_t borrow_in) {
    return fallback_sub_borrow_u64(a, b, borrow_in);
}

WIDEINT_HD inline mul_result gnu_arm64_mul_wide_u64(std::uint64_t a, std::uint64_t b) {
#if defined(__SIZEOF_INT128__)
    const unsigned __int128 product =
        static_cast<unsigned __int128>(a) * static_cast<unsigned __int128>(b);
    return {static_cast<std::uint64_t>(product),
            static_cast<std::uint64_t>(static_cast<unsigned __int128>(product >> 64))};
#else
    return fallback_mul_wide_u64(a, b);
#endif
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_GNU_ARM64_HPP
