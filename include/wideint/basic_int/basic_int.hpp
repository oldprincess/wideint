#ifndef WIDEINT_BASIC_INT_BASIC_INT_HPP
#define WIDEINT_BASIC_INT_BASIC_INT_HPP

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <wideint/basic_int/detail/common.hpp>
#include <wideint/basic_int/detail/limb_ops.hpp>

namespace wideint::detail {

template <std::size_t N, bool Signed> struct basic_int {
    std::uint64_t limbs[N]{};

    /** Defaulted constexpr so `std::numeric_limits` can form min/max at compile time. */
    constexpr basic_int() = default;

    template <class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
    WIDEINT_HD basic_int(T value) {
        assign_integral<N>(limbs, value);
    }

    template <
        class T0, class T1, class... Ts,
        class = std::enable_if_t<(is_integral_not_bool_v<T0> && is_integral_not_bool_v<T1> &&
                                  (... && is_integral_not_bool_v<Ts>)&&(2 + sizeof...(Ts) <= N))>>
    WIDEINT_HD basic_int(T0 v0, T1 v1, Ts... vs) {
        assign_limbs<N>(limbs, v0, v1, vs...);
    }

    template <std::size_t OtherN, bool OtherSigned,
              class = std::enable_if_t<OtherN != N || OtherSigned != Signed>>
    WIDEINT_HD basic_int(const basic_int<OtherN, OtherSigned> &other) {
        convert_basic_int_limbs<N, OtherN, OtherSigned>(limbs, other.limbs);
    }

    template <std::size_t OtherN, bool OtherSigned,
              class = std::enable_if_t<OtherN != N || OtherSigned != Signed>>
    WIDEINT_HD basic_int &operator=(const basic_int<OtherN, OtherSigned> &other) {
        convert_basic_int_limbs<N, OtherN, OtherSigned>(limbs, other.limbs);
        return *this;
    }

    WIDEINT_HD explicit operator bool() const { return !is_zero(limbs); }

    template <class T> WIDEINT_HD explicit operator T() const {
        static_assert(is_integral_not_bool_v<T>,
                      "basic_int only supports explicit conversion to integer types");
        return static_cast<T>(limbs[0]);
    }
};

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_BASIC_INT_HPP
