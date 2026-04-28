#include <cassert>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <wideint/bit.hpp>
#include <wideint/wideint.hpp>

using u128 = wideint::uint<2>;
using i128 = wideint::sint<2>;
using u256 = wideint::uint<4>;
using i256 = wideint::sint<4>;

static_assert(std::is_constructible_v<u256, u128>);
static_assert(std::is_constructible_v<u128, u256>);
static_assert(std::is_constructible_v<i256, i128>);
static_assert(std::is_constructible_v<i128, i256>);
static_assert(std::is_constructible_v<u128, i128>);
static_assert(std::is_constructible_v<i128, u128>);
static_assert(std::is_constructible_v<u256, i128>);
static_assert(std::is_constructible_v<i256, u128>);

// numeric_limits uses constexpr limb filling (no wideint intrinsics); verify limb pattern only.
static_assert(std::numeric_limits<u128>::max().limbs[0] == ~0ull &&
              std::numeric_limits<u128>::max().limbs[1] == ~0ull);
static_assert(std::numeric_limits<i128>::min().limbs[0] == 0 &&
              std::numeric_limits<i128>::min().limbs[1] == 0x8000000000000000ull);

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

    const u256 widened_u = u128{0xdeadbeefcafebabeull, 0x0123456789abcdefull};
    assert(widened_u.limbs[0] == 0xdeadbeefcafebabeull);
    assert(widened_u.limbs[1] == 0x0123456789abcdefull);
    assert(widened_u.limbs[2] == 0 && widened_u.limbs[3] == 0);

    const u128 narrow_u = u256{1u, 2u, 3u, 4u};
    assert(narrow_u.limbs[0] == 1 && narrow_u.limbs[1] == 2);

    const u256 from_neg_i = i128{-1};
    assert(from_neg_i.limbs[0] == ~0ull && from_neg_i.limbs[1] == ~0ull &&
           from_neg_i.limbs[2] == ~0ull && from_neg_i.limbs[3] == ~0ull);

    const i128 from_all_ones_u = all_ones;
    assert(from_all_ones_u == i128{-1});

    const i256 i_widen = i128{-5};
    const i128 narrow_i = i256{i_widen};
    assert(narrow_i == i128{-5});

    u256 assign_u{};
    assign_u = u128{7u, 8u};
    assert(assign_u.limbs[0] == 7 && assign_u.limbs[1] == 8 && assign_u.limbs[2] == 0);

    return 0;
}
