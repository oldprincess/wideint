#ifndef WIDEINT_WIDEINT_HPP
#define WIDEINT_WIDEINT_HPP

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "wideint/detail/common.hpp"
#include "wideint/detail/platform_ops.hpp"

namespace wideint {

namespace detail {

template <class T>
inline constexpr bool is_integral_not_bool_v =
    std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>;

template <std::size_t N> INTX_HD inline void zero_limbs(std::uint64_t (&dst)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = 0;
    }
}

template <std::size_t N> INTX_HD inline bool is_zero(const std::uint64_t (&limbs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        if (limbs[i] != 0) {
            return false;
        }
    }
    return true;
}

template <std::size_t N>
INTX_HD inline void copy_limbs(std::uint64_t (&dst)[N], const std::uint64_t (&src)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = src[i];
    }
}

template <std::size_t N>
INTX_HD inline int compare_unsigned(const std::uint64_t (&lhs)[N], const std::uint64_t (&rhs)[N]) {
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
INTX_HD inline int compare_signed(const std::uint64_t (&lhs)[N], const std::uint64_t (&rhs)[N]) {
    const bool lhs_negative = (lhs[N - 1] >> 63) != 0;
    const bool rhs_negative = (rhs[N - 1] >> 63) != 0;
    if (lhs_negative != rhs_negative) {
        return lhs_negative ? -1 : 1;
    }
    return compare_unsigned(lhs, rhs);
}

template <std::size_t N> INTX_HD inline void bit_not_assign(std::uint64_t (&dst)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] = ~dst[i];
    }
}

template <std::size_t N>
INTX_HD inline void bit_and_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] &= rhs[i];
    }
}

template <std::size_t N>
INTX_HD inline void bit_or_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] |= rhs[i];
    }
}

template <std::size_t N>
INTX_HD inline void bit_xor_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    for (std::size_t i = 0; i < N; ++i) {
        dst[i] ^= rhs[i];
    }
}

template <std::size_t N>
INTX_HD inline void add_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const auto sum = add_carry_u64(dst[i], rhs[i], carry);
        dst[i] = sum.value;
        carry = sum.carry;
    }
}

template <std::size_t N>
INTX_HD inline void sub_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t borrow = 0;
    for (std::size_t i = 0; i < N; ++i) {
        const auto diff = sub_borrow_u64(dst[i], rhs[i], borrow);
        dst[i] = diff.value;
        borrow = diff.borrow;
    }
}

template <std::size_t N> INTX_HD inline void negate_assign(std::uint64_t (&dst)[N]) {
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
INTX_HD inline void mul_assign(std::uint64_t (&dst)[N], const std::uint64_t (&rhs)[N]) {
    std::uint64_t lhs[N];
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
INTX_HD inline void shift_left_assign(std::uint64_t (&dst)[N], unsigned int shift) {
    constexpr unsigned int limb_bits = 64;
    const unsigned int total_bits = static_cast<unsigned int>(N * limb_bits);
    if (shift >= total_bits) {
        zero_limbs(dst);
        return;
    }

    const unsigned int limb_shift = shift / limb_bits;
    const unsigned int bit_shift = shift % limb_bits;
    std::uint64_t src[N];
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
INTX_HD inline void shift_right_assign_fill(std::uint64_t (&dst)[N], unsigned int shift,
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
    std::uint64_t src[N];
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
INTX_HD inline void assign_integral(std::uint64_t (&dst)[N], T value) {
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
INTX_HD inline void assign_limbs(std::uint64_t (&dst)[N], Args... args) {
    zero_limbs(dst);
    const std::uint64_t values[] = {static_cast<std::uint64_t>(args)...};
    for (std::size_t i = 0; i < sizeof...(Args); ++i) {
        dst[i] = values[i];
    }
}

template <bool Signed, std::size_t N>
INTX_HD inline int compare(const std::uint64_t (&lhs)[N], const std::uint64_t (&rhs)[N]) {
    if constexpr (Signed) {
        return compare_signed(lhs, rhs);
    } else {
        return compare_unsigned(lhs, rhs);
    }
}

template <bool Signed, std::size_t N>
INTX_HD inline std::uint64_t right_shift_fill(const std::uint64_t (&limbs)[N]) {
    if constexpr (Signed) {
        return (limbs[N - 1] >> 63) != 0 ? ~0ull : 0ull;
    } else {
        return 0;
    }
}

template <std::size_t N, bool Signed> struct basic_int {
    std::uint64_t limbs[N]{};

    INTX_HD constexpr basic_int() {}

    template <class T, class = std::enable_if_t<is_integral_not_bool_v<T>>>
    INTX_HD constexpr basic_int(T value) {
        assign_integral(limbs, value);
    }

    template <
        class T0, class T1, class... Ts,
        class = std::enable_if_t<(is_integral_not_bool_v<T0> && is_integral_not_bool_v<T1> &&
                                  (... && is_integral_not_bool_v<Ts>)&&(2 + sizeof...(Ts) <= N))>>
    INTX_HD constexpr basic_int(T0 v0, T1 v1, Ts... vs) {
        assign_limbs(limbs, v0, v1, vs...);
    }

    INTX_HD constexpr explicit operator bool() const { return !is_zero(limbs); }

    template <class T> INTX_HD constexpr explicit operator T() const {
        static_assert(is_integral_not_bool_v<T>,
                      "basic_int only supports explicit conversion to integer types");
        return static_cast<T>(limbs[0]);
    }

    INTX_HD basic_int &operator+=(const basic_int &rhs) {
        add_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator-=(const basic_int &rhs) {
        sub_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator*=(const basic_int &rhs) {
        mul_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator&=(const basic_int &rhs) {
        bit_and_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator|=(const basic_int &rhs) {
        bit_or_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator^=(const basic_int &rhs) {
        bit_xor_assign(limbs, rhs.limbs);
        return *this;
    }

    INTX_HD basic_int &operator<<=(unsigned int shift) {
        shift_left_assign(limbs, shift);
        return *this;
    }

    INTX_HD basic_int &operator>>=(unsigned int shift) {
        shift_right_assign_fill(limbs, shift, right_shift_fill<Signed>(limbs));
        return *this;
    }

    INTX_HD basic_int operator+() const { return *this; }

    INTX_HD basic_int operator-() const {
        basic_int out = *this;
        negate_assign(out.limbs);
        return out;
    }

    INTX_HD basic_int operator~() const {
        basic_int out = *this;
        bit_not_assign(out.limbs);
        return out;
    }

    INTX_HD basic_int &operator++() {
        *this += basic_int{1};
        return *this;
    }

    INTX_HD basic_int operator++(int) {
        basic_int out = *this;
        ++(*this);
        return out;
    }

    INTX_HD basic_int &operator--() {
        *this -= basic_int{1};
        return *this;
    }

    INTX_HD basic_int operator--(int) {
        basic_int out = *this;
        --(*this);
        return out;
    }
};

template <std::size_t N, bool Signed>
INTX_HD inline bool operator==(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return compare_unsigned(lhs.limbs, rhs.limbs) == 0;
}

template <std::size_t N, bool Signed>
INTX_HD inline bool operator!=(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return !(lhs == rhs);
}

template <std::size_t N, bool Signed>
INTX_HD inline bool operator<(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) < 0;
}

template <std::size_t N, bool Signed>
INTX_HD inline bool operator<=(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) <= 0;
}

template <std::size_t N, bool Signed>
INTX_HD inline bool operator>(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) > 0;
}

template <std::size_t N, bool Signed>
INTX_HD inline bool operator>=(const basic_int<N, Signed> &lhs, const basic_int<N, Signed> &rhs) {
    return compare<Signed>(lhs.limbs, rhs.limbs) >= 0;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator+(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs += rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator-(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs -= rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator*(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs *= rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator&(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs &= rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator|(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs |= rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator^(basic_int<N, Signed> lhs,
                                              const basic_int<N, Signed> &rhs) {
    lhs ^= rhs;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator<<(basic_int<N, Signed> lhs, unsigned int shift) {
    lhs <<= shift;
    return lhs;
}

template <std::size_t N, bool Signed>
INTX_HD inline basic_int<N, Signed> operator>>(basic_int<N, Signed> lhs, unsigned int shift) {
    lhs >>= shift;
    return lhs;
}

} // namespace detail

template <std::size_t N> using uint = detail::basic_int<N, false>;

template <std::size_t N> using sint = detail::basic_int<N, true>;

} // namespace wideint

#endif // WIDEINT_WIDEINT_HPP
