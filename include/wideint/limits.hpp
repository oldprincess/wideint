#ifndef WIDEINT_LIMITS_HPP
#define WIDEINT_LIMITS_HPP

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

#include <wideint/int.hpp>

namespace wideint {
namespace limits_detail {

template <std::size_t N> constexpr detail::basic_int<N, false> uint_max() noexcept {
    detail::basic_int<N, false> v{};
    for (std::size_t i = 0; i < N; ++i) {
        v.limbs[i] = ~std::uint64_t{};
    }
    return v;
}

template <std::size_t N> constexpr detail::basic_int<N, true> sint_max() noexcept {
    detail::basic_int<N, true> v{};
    for (std::size_t i = 0; i + 1 < N; ++i) {
        v.limbs[i] = ~std::uint64_t{};
    }
    v.limbs[N - 1] = 0x7fffffffffffffffull;
    return v;
}

template <std::size_t N> constexpr detail::basic_int<N, true> sint_min() noexcept {
    detail::basic_int<N, true> v{};
    v.limbs[N - 1] = 0x8000000000000000ull;
    return v;
}

} // namespace limits_detail
} // namespace wideint

namespace std {

template <> struct numeric_limits<wideint::uint<2>> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int digits = 128;
    static constexpr int digits10 = 38;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr wideint::uint<2> min() noexcept { return {}; }
    static constexpr wideint::uint<2> max() noexcept { return wideint::limits_detail::uint_max<2>(); }
    static constexpr wideint::uint<2> lowest() noexcept { return {}; }
};

template <> struct numeric_limits<wideint::sint<2>> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int digits = 127;
    static constexpr int digits10 = 38;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr wideint::sint<2> min() noexcept { return wideint::limits_detail::sint_min<2>(); }
    static constexpr wideint::sint<2> max() noexcept { return wideint::limits_detail::sint_max<2>(); }
    static constexpr wideint::sint<2> lowest() noexcept { return min(); }
};

template <> struct numeric_limits<wideint::uint<4>> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int digits = 256;
    static constexpr int digits10 = 77;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr wideint::uint<4> min() noexcept { return {}; }
    static constexpr wideint::uint<4> max() noexcept { return wideint::limits_detail::uint_max<4>(); }
    static constexpr wideint::uint<4> lowest() noexcept { return {}; }
};

template <> struct numeric_limits<wideint::sint<4>> {
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr int radix = 2;
    static constexpr int digits = 255;
    static constexpr int digits10 = 76;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr wideint::sint<4> min() noexcept { return wideint::limits_detail::sint_min<4>(); }
    static constexpr wideint::sint<4> max() noexcept { return wideint::limits_detail::sint_max<4>(); }
    static constexpr wideint::sint<4> lowest() noexcept { return min(); }
};

} // namespace std

#endif // WIDEINT_LIMITS_HPP
