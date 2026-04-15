#ifndef WIDEINT_BIT_HPP
#define WIDEINT_BIT_HPP

#pragma once

#include <cstddef>
#include <cstdint>

#if !defined(__CUDACC__) && defined(_MSC_VER)
#include <intrin.h>
#endif

#include <wideint/int.hpp>

namespace wideint {
namespace detail {

WIDEINT_HD constexpr inline unsigned int countl_zero_u64_fallback(std::uint64_t value) {
    unsigned int count = 0;
    for (std::uint64_t bit = 1ull << 63; bit != 0; bit >>= 1) {
        if ((value & bit) != 0) {
            break;
        }
        ++count;
    }
    return count;
}

WIDEINT_HD constexpr inline unsigned int countr_zero_u64_fallback(std::uint64_t value) {
    unsigned int count = 0;
    for (std::uint64_t bit = 1ull; bit != 0; bit <<= 1) {
        if ((value & bit) != 0) {
            break;
        }
        ++count;
    }
    return count;
}

WIDEINT_HD constexpr inline unsigned int popcount_u64_fallback(std::uint64_t value) {
    unsigned int count = 0;
    while (value != 0) {
        count += static_cast<unsigned int>(value & 1ull);
        value >>= 1;
    }
    return count;
}

WIDEINT_HD inline unsigned int countl_zero_u64(std::uint64_t value) {
#if defined(__CUDA_ARCH__)
    return countl_zero_u64_fallback(value);
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    unsigned long index = 0;
    _BitScanReverse64(&index, static_cast<unsigned __int64>(value));
    return 63u - static_cast<unsigned int>(index);
#elif defined(__clang__) || defined(__GNUC__)
    return static_cast<unsigned int>(__builtin_clzll(static_cast<unsigned long long>(value)));
#else
    return countl_zero_u64_fallback(value);
#endif
}

WIDEINT_HD inline unsigned int countr_zero_u64(std::uint64_t value) {
#if defined(__CUDA_ARCH__)
    return countr_zero_u64_fallback(value);
#elif !defined(__CUDACC__) && defined(_MSC_VER) && defined(_M_X64)
    unsigned long index = 0;
    _BitScanForward64(&index, static_cast<unsigned __int64>(value));
    return static_cast<unsigned int>(index);
#elif defined(__clang__) || defined(__GNUC__)
    return static_cast<unsigned int>(__builtin_ctzll(static_cast<unsigned long long>(value)));
#else
    return countr_zero_u64_fallback(value);
#endif
}

WIDEINT_HD inline unsigned int popcount_u64(std::uint64_t value) {
#if defined(__CUDA_ARCH__)
    return popcount_u64_fallback(value);
#elif defined(__clang__) || defined(__GNUC__)
    return static_cast<unsigned int>(__builtin_popcountll(static_cast<unsigned long long>(value)));
#else
    return popcount_u64_fallback(value);
#endif
}

} // namespace detail

WIDEINT_HD inline unsigned int countl_zero(std::uint64_t value) {
    return value == 0 ? 64u : detail::countl_zero_u64(value);
}

WIDEINT_HD inline unsigned int countr_zero(std::uint64_t value) {
    return value == 0 ? 64u : detail::countr_zero_u64(value);
}

WIDEINT_HD inline unsigned int popcount(std::uint64_t value) {
    return detail::popcount_u64(value);
}

WIDEINT_HD inline unsigned int bit_width(std::uint64_t value) {
    return value == 0 ? 0u : 64u - countl_zero(value);
}

WIDEINT_HD inline bool has_single_bit(std::uint64_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline unsigned int countl_zero(const detail::basic_int<N, Signed> &value) {
    unsigned int count = 0;
    for (std::size_t i = N; i-- > 0;) {
        if (value.limbs[i] != 0) {
            return count + countl_zero(value.limbs[i]);
        }
        count += 64u;
    }
    return count;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline unsigned int countr_zero(const detail::basic_int<N, Signed> &value) {
    unsigned int count = 0;
    for (std::size_t i = 0; i < N; ++i) {
        if (value.limbs[i] != 0) {
            return count + countr_zero(value.limbs[i]);
        }
        count += 64u;
    }
    return count;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline unsigned int popcount(const detail::basic_int<N, Signed> &value) {
    unsigned int count = 0;
    for (std::size_t i = 0; i < N; ++i) {
        count += popcount(value.limbs[i]);
    }
    return count;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline unsigned int bit_width(const detail::basic_int<N, Signed> &value) {
    return value ? static_cast<unsigned int>(64u * N) - countl_zero(value) : 0u;
}

template <std::size_t N, bool Signed>
WIDEINT_HD inline bool has_single_bit(const detail::basic_int<N, Signed> &value) {
    return popcount(value) == 1u;
}

} // namespace wideint

#endif // WIDEINT_BIT_HPP
