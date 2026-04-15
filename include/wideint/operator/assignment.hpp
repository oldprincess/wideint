#ifndef WIDEINT_OPERATOR_ASSIGNMENT_HPP
#define WIDEINT_OPERATOR_ASSIGNMENT_HPP

#pragma once

#include <type_traits>

#include <wideint/basic_int/basic_int.hpp>

namespace wideint::detail {

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator+=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    add_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator-=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    sub_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator*=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    mul_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator/=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    std::uint64_t q[N]{};
    std::uint64_t r[N]{};
    if constexpr (Signed) {
        signed_div_rem(q, r, lhs.limbs, rhs.limbs);
    } else {
        unsigned_div_rem(q, r, lhs.limbs, rhs.limbs);
    }
    copy_limbs(lhs.limbs, q);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator%=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    std::uint64_t q[N]{};
    std::uint64_t r[N]{};
    if constexpr (Signed) {
        signed_div_rem(q, r, lhs.limbs, rhs.limbs);
    } else {
        unsigned_div_rem(q, r, lhs.limbs, rhs.limbs);
    }
    copy_limbs(lhs.limbs, r);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator&=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    bit_and_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator|=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    bit_or_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator^=(basic_int<N, Signed> &lhs,
                                                  const basic_int<N, Signed> &rhs) {
    bit_xor_assign(lhs.limbs, rhs.limbs);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator<<=(basic_int<N, Signed> &lhs,
                                                   unsigned int shift) {
    shift_left_assign(lhs.limbs, shift);
    return lhs;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator>>=(basic_int<N, Signed> &lhs,
                                                   unsigned int shift) {
    shift_right_assign_fill(lhs.limbs, shift, right_shift_fill<Signed>(lhs.limbs));
    return lhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator+=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs += basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator-=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs -= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator*=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs *= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator/=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs /= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator%=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs %= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator&=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs &= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator|=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs |= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator^=(basic_int<N, Signed> &lhs, T rhs) {
    return lhs ^= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator<<=(basic_int<N, Signed> &lhs, T shift) {
    return lhs <<= static_cast<unsigned int>(shift);
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline basic_int<N, Signed> &operator>>=(basic_int<N, Signed> &lhs, T shift) {
    return lhs >>= static_cast<unsigned int>(shift);
}

} // namespace wideint::detail

#endif // WIDEINT_OPERATOR_ASSIGNMENT_HPP
