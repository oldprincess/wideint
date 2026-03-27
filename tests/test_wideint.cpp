#include <cassert>
#include <cstdint>
#include <type_traits>

#include "wideint/bit.hpp"
#include "wideint/wideint.hpp"

using u128 = wideint::uint<2>;
using i128 = wideint::sint<2>;

static_assert(sizeof(u128) == 16, "uint<2> must occupy two 64-bit limbs");
static_assert(sizeof(i128) == 16, "sint<2> must occupy two 64-bit limbs");
static_assert(std::is_standard_layout_v<u128>, "uint<2> must be standard layout");
static_assert(std::is_standard_layout_v<i128>, "sint<2> must be standard layout");
static_assert(std::is_constructible_v<std::uint32_t, u128>,
              "unsigned integers must support explicit uint32_t conversion");
static_assert(std::is_constructible_v<std::int32_t, u128>,
              "unsigned integers must support explicit int32_t conversion");
static_assert(std::is_constructible_v<std::uint32_t, i128>,
              "signed integers must support explicit uint32_t conversion");
static_assert(std::is_constructible_v<std::int32_t, i128>,
              "signed integers must support explicit int32_t conversion");
static_assert(std::is_constructible_v<std::uint64_t, u128>,
              "unsigned integers must support explicit uint64_t conversion");
static_assert(std::is_constructible_v<std::int64_t, u128>,
              "unsigned integers must support explicit int64_t conversion");
static_assert(std::is_constructible_v<std::uint64_t, i128>,
              "signed integers must support explicit uint64_t conversion");
static_assert(std::is_constructible_v<std::int64_t, i128>,
              "signed integers must support explicit int64_t conversion");

int main() {
    const u128 zero{};
    const u128 one{1u};
    const u128 top_bit{0u, 1u};
    const u128 all_ones{~0ull, ~0ull};
    const u128 one_or_top{1u, 1u};

    const u128 sum = all_ones + one;
    assert(sum == zero);

    const u128 diff = zero - one;
    assert(diff == all_ones);

    const u128 mul_a{0xffffffffffffffffull, 0ull};
    const u128 mul_b{2u};
    const u128 mul_expected{0xfffffffffffffffeull, 1u};
    assert(mul_a * mul_b == mul_expected);

    assert((top_bit >> 64) == one);
    assert(((one << 65) >> 65) == one);

    assert((all_ones & top_bit) == top_bit);
    assert((one | top_bit) == one_or_top);
    assert((one ^ top_bit) == one_or_top);

    const i128 neg_one{-1};
    const i128 neg_two{-2};
    const i128 pos_one{1};
    assert((neg_one >> 1) == neg_one);
    assert((neg_two >> 1) == neg_one);
    assert(neg_one < pos_one);
    assert(pos_one > neg_one);
    assert(i128{-3} + i128{1} == i128{-2});
    assert(-pos_one == neg_one);

    u128 inc{0u};
    ++inc;
    assert(inc == one);
    --inc;
    assert(inc == zero);

    assert(static_cast<bool>(one));
    assert(!static_cast<bool>(zero));

    assert(static_cast<std::uint32_t>(u128{0x1234u, 0x5678u}) == 0x1234u);
    assert(static_cast<std::int32_t>(i128{-1}) == -1);
    assert(static_cast<std::uint64_t>(u128{0x1234u, 0x5678u}) == 0x1234u);
    assert(static_cast<std::int64_t>(u128{0x1234u, 0x5678u}) == 0x1234u);
    assert(static_cast<std::uint64_t>(i128{-1}) == ~0ull);
    assert(static_cast<std::int64_t>(i128{-1}) == -1);

    static_assert(std::is_same_v<decltype(i128{1} + 1), i128>);
    static_assert(std::is_same_v<decltype(1 + i128{1}), i128>);
    static_assert(std::is_same_v<decltype(i128{4} - 3), i128>);
    static_assert(std::is_same_v<decltype(4 - i128{3}), i128>);
    static_assert(std::is_same_v<decltype(i128{4} * 3), i128>);
    static_assert(std::is_same_v<decltype(3 * i128{4}), i128>);
    static_assert(std::is_same_v<decltype(u128{7} & 3u), u128>);
    static_assert(std::is_same_v<decltype(3u & u128{7}), u128>);
    static_assert(std::is_same_v<decltype(u128{7} | 3u), u128>);
    static_assert(std::is_same_v<decltype(3u | u128{7}), u128>);
    static_assert(std::is_same_v<decltype(u128{7} ^ 3u), u128>);
    static_assert(std::is_same_v<decltype(3u ^ u128{7}), u128>);

    i128 mixed{5};
    mixed = mixed + 1;
    assert(mixed == 6);
    mixed = 1 + mixed;
    assert(mixed == 7);
    mixed = mixed - 2;
    assert(mixed == 5);
    mixed = 9 - mixed;
    assert(mixed == 4);
    mixed = mixed * 3;
    assert(mixed == 12);
    mixed = 2 * mixed;
    assert(mixed == 24);
    assert(mixed == 24);
    assert(24 == mixed);
    assert(mixed != 23);
    assert(23 != mixed);
    assert(mixed > 20);
    assert(20 < mixed);
    assert(mixed >= 24);
    assert(24 <= mixed);

    u128 mask{6u};
    assert((mask & 3u) == u128{2u});
    assert((3u & mask) == u128{2u});
    assert((mask | 1u) == u128{7u});
    assert((1u | mask) == u128{7u});
    assert((mask ^ 3u) == u128{5u});
    assert((3u ^ mask) == u128{5u});

    assert(wideint::countl_zero(std::uint64_t{0}) == 64);
    assert(wideint::countr_zero(std::uint64_t{0}) == 64);
    assert(wideint::popcount(std::uint64_t{0}) == 0);
    assert(wideint::bit_width(std::uint64_t{0}) == 0);
    assert(!wideint::has_single_bit(std::uint64_t{0}));
    assert(wideint::countl_zero(std::uint64_t{1}) == 63);
    assert(wideint::countl_zero(std::uint64_t{1} << 63) == 0);
    assert(wideint::countr_zero(std::uint64_t{8}) == 3);
    assert(wideint::countr_zero(std::uint64_t{1} << 63) == 63);
    assert(wideint::popcount(std::uint64_t{0xf0}) == 4);
    assert(wideint::popcount(~std::uint64_t{0}) == 64);
    assert(wideint::bit_width(std::uint64_t{9}) == 4);
    assert(wideint::has_single_bit(std::uint64_t{8}));

    assert(wideint::countl_zero(zero) == 128);
    assert(wideint::countl_zero(one) == 127);
    assert(wideint::countl_zero(top_bit) == 63);
    assert(wideint::countr_zero(zero) == 128);
    assert(wideint::countr_zero(top_bit) == 64);
    assert(wideint::countr_zero(u128{8u}) == 3);
    assert(wideint::popcount(zero) == 0);
    assert(wideint::popcount(all_ones) == 128);
    assert(wideint::popcount(one_or_top) == 2);
    assert(wideint::bit_width(zero) == 0);
    assert(wideint::bit_width(one) == 1);
    assert(wideint::bit_width(top_bit) == 65);
    assert(wideint::has_single_bit(one));
    assert(wideint::has_single_bit(top_bit));
    assert(!wideint::has_single_bit(zero));
    assert(!wideint::has_single_bit(one_or_top));

    assert(wideint::countl_zero(i128{-1}) == 0);
    assert(wideint::countr_zero(i128{-2}) == 1);
    assert(wideint::popcount(i128{-1}) == 128);
    assert(wideint::bit_width(i128{-1}) == 128);
    assert(wideint::has_single_bit(i128{1}));

    return 0;
}
