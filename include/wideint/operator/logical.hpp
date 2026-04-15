#ifndef WIDEINT_OPERATOR_LOGICAL_HPP
#define WIDEINT_OPERATOR_LOGICAL_HPP

#pragma once

#include <wideint/basic_int/basic_int.hpp>

namespace wideint::detail {

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool operator!(const basic_int<N, Signed> &value) {
    return !static_cast<bool>(value);
}

} // namespace wideint::detail

#endif // WIDEINT_OPERATOR_LOGICAL_HPP
