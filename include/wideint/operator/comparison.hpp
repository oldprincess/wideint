#ifndef WIDEINT_OPERATOR_COMPARISON_HPP
#define WIDEINT_OPERATOR_COMPARISON_HPP

#pragma once

#include <type_traits>

#include <wideint/basic_int/basic_int.hpp>

namespace wideint::detail {

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator==(const basic_int<N, Signed> &lhs,
                                 const basic_int<N, Signed> &rhs) {
    return compare_unsigned(lhs.limbs, rhs.limbs) == 0;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator!=(const basic_int<N, Signed> &lhs,
                                 const basic_int<N, Signed> &rhs) {
    return !(lhs == rhs);
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator<(const basic_int<N, Signed> &lhs,
                                const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) < 0;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator<=(const basic_int<N, Signed> &lhs,
                                 const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) <= 0;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator>(const basic_int<N, Signed> &lhs,
                                const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) > 0;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator>=(const basic_int<N, Signed> &lhs,
                                 const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) >= 0;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator==(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs == basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator==(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} == rhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator!=(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs != basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator!=(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} != rhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator<(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs < basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator<(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} < rhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator<=(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs <= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator<=(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} <= rhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator>(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs > basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator>(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} > rhs;
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator>=(const basic_int<N, Signed> &lhs, T rhs) {
    return lhs >= basic_int<N, Signed>{rhs};
}

template <std::size_t N, bool Signed, class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
WIDEINT_HD inline bool operator>=(T lhs, const basic_int<N, Signed> &rhs) {
    return basic_int<N, Signed>{lhs} >= rhs;
}

} // namespace wideint::detail

#endif // WIDEINT_OPERATOR_COMPARISON_HPP
