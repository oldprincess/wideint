#include <cassert>
#include <cstdint>
#include <type_traits>

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

    return 0;
}