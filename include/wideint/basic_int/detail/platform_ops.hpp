#ifndef WIDEINT_BASIC_INT_DETAIL_PLATFORM_OPS_HPP
#define WIDEINT_BASIC_INT_DETAIL_PLATFORM_OPS_HPP

#pragma once

#include <cstdint>

#include <wideint/basic_int/detail/common.hpp>
#include <wideint/basic_int/detail/fallback.hpp>

#if defined(__CUDA_ARCH__)
#include <wideint/basic_int/detail/cuda.hpp>
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
#include <wideint/basic_int/detail/msvc_x86_64.hpp>
#elif !defined(__CUDACC__) && (defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
#include <wideint/basic_int/detail/gnu_x86_64.hpp>
#elif !defined(__CUDACC__) && defined(__aarch64__)
#include <wideint/basic_int/detail/gnu_arm64.hpp>
#endif

namespace wideint::detail {

WIDEINT_HD inline add_result add_carry_u64(std::uint64_t a, std::uint64_t b,
                                           std::uint64_t carry_in) {
#if defined(__CUDA_ARCH__)
    return fallback_add_carry_u64(a, b, carry_in);
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    return msvc_x86_64_add_carry_u64(a, b, carry_in);
#elif !defined(__CUDACC__) && (defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
    return gnu_x86_64_add_carry_u64(a, b, carry_in);
#elif !defined(__CUDACC__) && defined(__aarch64__)
    return gnu_arm64_add_carry_u64(a, b, carry_in);
#else
    return fallback_add_carry_u64(a, b, carry_in);
#endif
}

WIDEINT_HD inline sub_result sub_borrow_u64(std::uint64_t a, std::uint64_t b,
                                            std::uint64_t borrow_in) {
#if defined(__CUDA_ARCH__)
    return fallback_sub_borrow_u64(a, b, borrow_in);
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    return msvc_x86_64_sub_borrow_u64(a, b, borrow_in);
#elif !defined(__CUDACC__) && (defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
    return gnu_x86_64_sub_borrow_u64(a, b, borrow_in);
#elif !defined(__CUDACC__) && defined(__aarch64__)
    return gnu_arm64_sub_borrow_u64(a, b, borrow_in);
#else
    return fallback_sub_borrow_u64(a, b, borrow_in);
#endif
}

WIDEINT_HD inline mul_result mul_wide_u64(std::uint64_t a, std::uint64_t b) {
#if defined(__CUDA_ARCH__)
    return cuda_mul_wide_u64(a, b);
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    return msvc_x86_64_mul_wide_u64(a, b);
#elif !defined(__CUDACC__) && (defined(__GNUC__) || defined(__clang__)) && defined(__x86_64__)
    return gnu_x86_64_mul_wide_u64(a, b);
#elif !defined(__CUDACC__) && defined(__aarch64__)
    return gnu_arm64_mul_wide_u64(a, b);
#else
    return fallback_mul_wide_u64(a, b);
#endif
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_PLATFORM_OPS_HPP
