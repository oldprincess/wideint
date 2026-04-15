#ifndef WIDEINT_OPERATOR_INCREMENT_DECREMENT_HPP
#define WIDEINT_OPERATOR_INCREMENT_DECREMENT_HPP

#pragma once

#include <wideint/basic_int/basic_int.hpp>

namespace wideint::detail {

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator++(basic_int<N, Signed> &value) {
    std::uint64_t one[N]{};
    assign_integral<N>(one, static_cast<std::uint64_t>(1));
    add_assign(value.limbs, one);
    return value;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> operator++(basic_int<N, Signed> &value, int) {
    const basic_int<N, Signed> before = value;
    ++value;
    return before;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> &operator--(basic_int<N, Signed> &value) {
    std::uint64_t one[N]{};
    assign_integral<N>(one, static_cast<std::uint64_t>(1));
    sub_assign(value.limbs, one);
    return value;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline basic_int<N, Signed> operator--(basic_int<N, Signed> &value, int) {
    const basic_int<N, Signed> before = value;
    --value;
    return before;
}

} // namespace wideint::detail

#endif // WIDEINT_OPERATOR_INCREMENT_DECREMENT_HPP
