#ifndef WIDEINT_BASIC_INT_DETAIL_LIMB_OPS_HPP
#define WIDEINT_BASIC_INT_DETAIL_LIMB_OPS_HPP

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <wideint/basic_int/detail/platform_ops.hpp>

namespace wideint::detail {

template <class T>
inline constexpr bool is_integral_not_bool_v =
    std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>;

template <std::size_t N> WIDEINT_HD inline void zero_limbs(std::uint64_t (&dst)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = 0;
    }
}

template <std::size_t N> WIDEINT_HD inline bool is_zero(const std::uint64_t (&limbs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        if (limbs[i] != 0) {
            return false;
        }
    }
    return true;
}

template <std::size_t N>
WIDEINT_HD inline void copy_limbs(std::uint64_t (&dst)[N], const std::uint64_t (&src)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = src[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline int compare_unsigned(const std::uint64_t (&lhs)[N],
                                       const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = N; i-- > 0;) {
        if (lhs[i] < rhs[i]) {
            return -1;
        }
        if (lhs[i] > rhs[i]) {
            return 1;
        }
    }
    return 0;
}

template <std::size_t N>
WIDEINT_HD inline int compare_signed(const std::uint64_t (&lhs)[N], const std::uint64_t (&rhs)[N]) {
    const bool lhs_negative = (lhs[N - 1] >> 63) != 0;
    const bool rhs_negative = (rhs[N - 1] >> 63) != 0;
    if (lhs_negative != rhs_negative) {
        return lhs_negative ? -1 : 1;
    }
    return compare_unsigned(lhs, rhs);
}

template <std::size_t N> WIDEINT_HD inline void bit_not_assign(std::uint64_t (&dst)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = ~dst[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline void bit_and_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] &= rhs[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline void bit_or_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] |= rhs[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline void bit_xor_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] ^= rhs[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline void add_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const auto sum = add_carry_u64(dst[i], rhs[i], carry);
        dst[i] = sum.value;
        carry = sum.carry;
    }
}

template <std::size_t N>
WIDEINT_HD inline void sub_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t borrow = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const auto diff = sub_borrow_u64(dst[i], rhs[i], borrow);
        dst[i] = diff.value;
        borrow = diff.borrow;
    }
}

template <std::size_t N> WIDEINT_HD inline void negate_assign(std::uint64_t (&dst)[N]) {
    bit_not_assign(dst);
    std::uint64_t carry = 1;
    for (std::size_t i = 0; i < N; ++i) {
        const auto sum = add_carry_u64(dst[i], 0, carry);
        dst[i] = sum.value;
        carry = sum.carry;
        if (carry == 0) {
            break;
        }
    }
}

template <std::size_t N>
WIDEINT_HD inline void mul_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t lhs[N]{};
    copy_limbs(lhs, dst);
    zero_limbs(dst);

    for (std::size_t i = 0; i < N; ++i) {
        std::uint64_t carry_lo = 0;
        std::uint64_t carry_hi = 0;
        for (std::size_t j = 0; j + i < N; ++j) {
            const std::size_t k = i + j;
            const auto prod = mul_wide_u64(lhs[i], rhs[j]);

            const auto s0 = add_carry_u64(dst[k], prod.lo, 0);
            const auto s1 = add_carry_u64(s0.value, carry_lo, 0);
            dst[k] = s1.value;

            const auto c0 = add_carry_u64(prod.hi, s0.carry, 0);
            const auto c1 = add_carry_u64(c0.value, s1.carry, 0);
            const auto c2 = add_carry_u64(c1.value, carry_hi, 0);
            carry_lo = c2.value;
            carry_hi = c0.carry | c1.carry | c2.carry;
        }
    }
}

template <std::size_t N>
WIDEINT_HD inline void shift_left_assign(std::uint64_t (&dst)[N], unsigned int shift) {
    constexpr unsigned int limb_bits = 64;
    const unsigned int total_bits = static_cast<unsigned int>(N * limb_bits);
    if (shift >= total_bits) {
        zero_limbs(dst);
        return;
    }

    const unsigned int limb_shift = shift / limb_bits;
    const unsigned int bit_shift = shift % limb_bits;
    std::uint64_t src[N]{};
    copy_limbs(src, dst);

    for (std::size_t i = 0; i < N; ++i) {
        std::uint64_t value = 0;
        if (i >= limb_shift) {
            value = src[i - limb_shift] << bit_shift;
            if (bit_shift != 0 && i > limb_shift) {
                value |= src[i - limb_shift - 1] >> (limb_bits - bit_shift);
            }
        }
        dst[i] = value;
    }
}

template <std::size_t N>
WIDEINT_HD inline void shift_right_assign_fill(std::uint64_t (&dst)[N], unsigned int shift,
                                               std::uint64_t fill) {
    constexpr unsigned int limb_bits = 64;
    const unsigned int total_bits = static_cast<unsigned int>(N * limb_bits);
    if (shift >= total_bits) {
        for (std::size_t i = 0; i < N; ++i) {
            dst[i] = fill;
        }
        return;
    }

    const unsigned int limb_shift = shift / limb_bits;
    const unsigned int bit_shift = shift % limb_bits;
    std::uint64_t src[N]{};
    copy_limbs(src, dst);

    for (std::size_t i = 0; i < N; ++i) {
        const std::size_t low_index = i + limb_shift;
        const std::uint64_t low = low_index < N ? src[low_index] : fill;
        std::uint64_t value = bit_shift == 0 ? low : (low >> bit_shift);
        if (bit_shift != 0) {
            const std::size_t high_index = low_index + 1;
            const std::uint64_t high = high_index < N ? src[high_index] : fill;
            value |= high << (limb_bits - bit_shift);
        }
        dst[i] = value;
    }
}

template <std::size_t N, class T>
WIDEINT_HD inline void assign_integral(std::uint64_t (&dst)[N], T value) {
    zero_limbs(dst);
    dst[0] = static_cast<std::uint64_t>(value);
    if constexpr (std::is_signed_v<T>) {
        if (value < 0) {
            for (std::size_t i = 1; i < N; ++i) {
                dst[i] = ~0ull;
            }
        }
    }
}

template <std::size_t N, class... Args>
WIDEINT_HD inline void assign_limbs(std::uint64_t (&dst)[N], Args... args) {
    zero_limbs(dst);
    const std::uint64_t values[] = {static_cast<std::uint64_t>(args)...};
    for (std::size_t i = 0; i < sizeof...(Args); ++i) {
        dst[i] = values[i];
    }
}

/** Resize limb array like C++ integer conversion: narrow keeps low limbs; widen uses sign
 *  extension when SrcSigned, otherwise zero extension. */
template <std::size_t DstN, std::size_t SrcN, bool SrcSigned>
WIDEINT_HD inline void convert_basic_int_limbs(std::uint64_t (&dst)[DstN],
                                               const std::uint64_t (&src)[SrcN]) {
    constexpr std::size_t copy_n = DstN < SrcN ? DstN : SrcN;
    for (std::size_t i = 0; i < copy_n; ++i) {
        dst[i] = src[i];
    }
    if constexpr (DstN > SrcN) {
        std::uint64_t fill = 0;
        if constexpr (SrcSigned) {
            if ((src[SrcN - 1] >> 63) != 0) {
                fill = ~std::uint64_t{0};
            }
        }
        for (std::size_t i = SrcN; i < DstN; ++i) {
            dst[i] = fill;
        }
    }
}

template <bool Signed, std::size_t N>
WIDEINT_HD inline int compare(const std::uint64_t (&lhs)[N], const std::uint64_t (&rhs)[N]) {
    if constexpr (Signed) {
        return compare_signed(lhs, rhs);
    } else {
        return compare_unsigned(lhs, rhs);
    }
}

template <bool Signed, std::size_t N>
WIDEINT_HD inline std::uint64_t right_shift_fill(const std::uint64_t (&limbs)[N]) {
    if constexpr (Signed) {
        return (limbs[N - 1] >> 63) != 0 ? ~0ull : 0ull;
    } else {
        return 0;
    }
}

template <std::size_t N>
WIDEINT_HD inline void set_bit(std::uint64_t (&dst)[N], unsigned int bit_index) {
    dst[bit_index / 64u] |= (1ull << (bit_index % 64u));
}

template <std::size_t N>
WIDEINT_HD inline void unsigned_div_rem(std::uint64_t (&q)[N], std::uint64_t (&r)[N],
                                        const std::uint64_t (&u)[N], const std::uint64_t (&v)[N]) {
    zero_limbs(q);
    if (is_zero(v)) {
        return;
    }
    if (compare_unsigned(u, v) < 0) {
        copy_limbs(r, u);
        return;
    }

    constexpr std::size_t NP1 = N + 1;
    std::uint64_t vext[NP1]{};
    for (std::size_t i = 0; i < N; ++i) {
        vext[i] = v[i];
    }

    std::uint64_t R[NP1]{};
    for (int k = static_cast<int>(N * 64) - 1; k >= 0; --k) {
        std::uint64_t carry = 0;
        for (std::size_t i = 0; i < NP1; ++i) {
            const std::uint64_t cur = R[i];
            R[i] = (cur << 1) | carry;
            carry = cur >> 63;
        }
        const unsigned int limb = static_cast<unsigned int>(k / 64);
        const unsigned int bit = static_cast<unsigned int>(k % 64);
        R[0] |= (u[limb] >> bit) & 1;

        if (compare_unsigned(R, vext) >= 0) {
            sub_assign(R, vext);
            set_bit(q, static_cast<unsigned int>(k));
        }
    }
    for (std::size_t i = 0; i < N; ++i) {
        r[i] = R[i];
    }
}

template <std::size_t N>
WIDEINT_HD inline void signed_div_rem(std::uint64_t (&q)[N], std::uint64_t (&r)[N],
                                      const std::uint64_t (&u)[N], const std::uint64_t (&v)[N]) {
    const bool u_neg = (u[N - 1] >> 63) != 0;
    const bool v_neg = (v[N - 1] >> 63) != 0;

    std::uint64_t ua[N]{};
    std::uint64_t va[N]{};
    copy_limbs(ua, u);
    copy_limbs(va, v);
    if (u_neg) {
        negate_assign(ua);
    }
    if (v_neg) {
        negate_assign(va);
    }

    unsigned_div_rem(q, r, ua, va);

    if (!is_zero(r) && u_neg) {
        negate_assign(r);
    }
    if (u_neg ^ v_neg) {
        negate_assign(q);
    }
}

} // namespace wideint::detail

#endif // WIDEINT_BASIC_INT_DETAIL_LIMB_OPS_HPP
