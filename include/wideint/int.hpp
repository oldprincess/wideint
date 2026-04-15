#ifndef WIDEINT_INT_HPP
#define WIDEINT_INT_HPP

#pragma once

#include <wideint/basic_int/basic_int.hpp>

namespace wideint {

template <std::size_t N> using uint = detail::basic_int<N, false>;

template <std::size_t N> using sint = detail::basic_int<N, true>;

using uint128_t = uint<2>;
using int128_t = sint<2>;

// 196- and 256-bit values both use four 64-bit limbs; uint196_t/uint256_t name the same width.
using uint196_t = uint<4>;
using int196_t = sint<4>;
using uint256_t = uint<4>;
using int256_t = sint<4>;

} // namespace wideint

#endif // WIDEINT_INT_HPP
