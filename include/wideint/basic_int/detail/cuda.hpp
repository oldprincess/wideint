#ifndef WIDEINT_BASIC_INT_DETAIL_CUDA_HPP
#define WIDEINT_BASIC_INT_DETAIL_CUDA_HPP

#pragma once

#include <cstdint>

#include <wideint/basic_int/detail/common.hpp>

namespace wideint::detail {

#if defined(__CUDA_ARCH__)

WIDEINT_HD inline mul_result cuda_mul_wide_u64(std::uint64_t a, std::uint64_t b) {
    return {a * b, __umul64hi(a, b)};
}

#endif // defined(__CUDA_ARCH__)

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_CUDA_HPP
