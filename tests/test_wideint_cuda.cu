#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

#include <cuda_runtime.h>

#include "wideint/bit.hpp"
#include "wideint/wideint.hpp"

using u128 = wideint::uint<2>;
using i128 = wideint::sint<2>;

static_assert(sizeof(u128) == 16, "uint<2> must occupy two 64-bit limbs");
static_assert(sizeof(i128) == 16, "sint<2> must occupy two 64-bit limbs");
static_assert(std::is_standard_layout_v<u128>, "uint<2> must be standard layout");
static_assert(std::is_standard_layout_v<i128>, "sint<2> must be standard layout");

namespace {

__device__ void expect(bool condition, int *failures) {
    if (!condition) {
        atomicAdd(failures, 1);
    }
}

__global__ void run_wideint_cuda_tests(int *failures) {
    const u128 zero{};
    const u128 one{1u};
    const u128 top_bit{0u, 1u};
    const u128 all_ones{~0ull, ~0ull};
    const u128 one_or_top{1u, 1u};
    const u128 sum = all_ones + one;

    expect(sum == zero, failures);
    expect((zero - one) == all_ones, failures);

    const u128 mul_a{0xffffffffffffffffull, 0ull};
    const u128 mul_b{2u};
    const u128 mul_expected{0xfffffffffffffffeull, 1u};
    expect((mul_a * mul_b) == mul_expected, failures);

    expect((top_bit >> 64) == one, failures);
    expect(((one << 65) >> 65) == one, failures);

    expect((all_ones & top_bit) == top_bit, failures);
    expect((one | top_bit) == one_or_top, failures);
    expect((one ^ top_bit) == one_or_top, failures);

    const i128 neg_one{-1};
    const i128 neg_two{-2};
    const i128 pos_one{1};
    expect((neg_one >> 1) == neg_one, failures);
    expect((neg_two >> 1) == neg_one, failures);
    expect(neg_one < pos_one, failures);
    expect(pos_one > neg_one, failures);
    expect(i128{-3} + i128{1} == i128{-2}, failures);
    expect(-pos_one == neg_one, failures);

    u128 inc{0u};
    ++inc;
    expect(inc == one, failures);
    --inc;
    expect(inc == zero, failures);

    expect(static_cast<bool>(one), failures);
    expect(!static_cast<bool>(zero), failures);

    expect(static_cast<std::uint64_t>(u128{0x1234u, 0x5678u}) == 0x1234u, failures);
    expect(static_cast<std::int64_t>(i128{-1}) == -1, failures);
    expect(alignof(u128) == alignof(std::uint64_t), failures);
    expect(alignof(i128) == alignof(std::uint64_t), failures);
    expect(offsetof(u128, limbs) == 0, failures);
    expect(offsetof(i128, limbs) == 0, failures);

    i128 mixed{5};
    mixed = mixed + 1;
    expect(mixed == 6, failures);
    mixed = 1 + mixed;
    expect(mixed == 7, failures);
    mixed = mixed - 2;
    expect(mixed == 5, failures);
    mixed = 9 - mixed;
    expect(mixed == 4, failures);
    mixed = mixed * 3;
    expect(mixed == 12, failures);
    mixed = 2 * mixed;
    expect(mixed == 24, failures);
    expect(mixed == 24, failures);
    expect(24 == mixed, failures);
    expect(mixed != 23, failures);
    expect(23 != mixed, failures);
    expect(mixed > 20, failures);
    expect(20 < mixed, failures);
    expect(mixed >= 24, failures);
    expect(24 <= mixed, failures);

    u128 mask{6u};
    expect((mask & 3u) == u128{2u}, failures);
    expect((3u & mask) == u128{2u}, failures);
    expect((mask | 1u) == u128{7u}, failures);
    expect((1u | mask) == u128{7u}, failures);
    expect((mask ^ 3u) == u128{5u}, failures);
    expect((3u ^ mask) == u128{5u}, failures);

    expect(wideint::countl_zero(std::uint64_t{0}) == 64, failures);
    expect(wideint::countr_zero(std::uint64_t{0}) == 64, failures);
    expect(wideint::popcount(std::uint64_t{0}) == 0, failures);
    expect(wideint::bit_width(std::uint64_t{0}) == 0, failures);
    expect(!wideint::has_single_bit(std::uint64_t{0}), failures);
    expect(wideint::countl_zero(std::uint64_t{1}) == 63, failures);
    expect(wideint::countl_zero(std::uint64_t{1} << 63) == 0, failures);
    expect(wideint::countr_zero(std::uint64_t{8}) == 3, failures);
    expect(wideint::countr_zero(std::uint64_t{1} << 63) == 63, failures);
    expect(wideint::popcount(std::uint64_t{0xf0}) == 4, failures);
    expect(wideint::popcount(~std::uint64_t{0}) == 64, failures);
    expect(wideint::bit_width(std::uint64_t{9}) == 4, failures);
    expect(wideint::has_single_bit(std::uint64_t{8}), failures);

    expect(wideint::countl_zero(zero) == 128, failures);
    expect(wideint::countl_zero(one) == 127, failures);
    expect(wideint::countl_zero(top_bit) == 63, failures);
    expect(wideint::countr_zero(zero) == 128, failures);
    expect(wideint::countr_zero(top_bit) == 64, failures);
    expect(wideint::countr_zero(u128{8u}) == 3, failures);
    expect(wideint::popcount(zero) == 0, failures);
    expect(wideint::popcount(all_ones) == 128, failures);
    expect(wideint::popcount(one_or_top) == 2, failures);
    expect(wideint::bit_width(zero) == 0, failures);
    expect(wideint::bit_width(one) == 1, failures);
    expect(wideint::bit_width(top_bit) == 65, failures);
    expect(wideint::has_single_bit(one), failures);
    expect(wideint::has_single_bit(top_bit), failures);
    expect(!wideint::has_single_bit(zero), failures);
    expect(!wideint::has_single_bit(one_or_top), failures);

    expect(wideint::countl_zero(i128{-1}) == 0, failures);
    expect(wideint::countr_zero(i128{-2}) == 1, failures);
    expect(wideint::popcount(i128{-1}) == 128, failures);
    expect(wideint::bit_width(i128{-1}) == 128, failures);
    expect(wideint::has_single_bit(i128{1}), failures);
}

int run_wideint_cuda_host_tests() {
    const u128 one{1u};
    const u128 top_bit{0u, 1u};
    const u128 sum = top_bit + one;

    if (sum != u128{1u, 1u}) {
        return 1;
    }
    if (wideint::bit_width(sum) != 65) {
        return 2;
    }
    if (!wideint::has_single_bit(top_bit)) {
        return 3;
    }
    if (wideint::countr_zero(top_bit) != 64) {
        return 4;
    }
    return 0;
}

void check_cuda(cudaError_t status) {
    assert(status == cudaSuccess);
}

} // namespace

int main() {
    int failures = 0;
    int *device_failures = nullptr;
    const int host_failures = run_wideint_cuda_host_tests();

    assert(host_failures == 0);

    check_cuda(cudaMalloc(&device_failures, sizeof(int)));
    check_cuda(cudaMemcpy(device_failures, &failures, sizeof(int), cudaMemcpyHostToDevice));

    run_wideint_cuda_tests<<<1, 1>>>(device_failures);
    check_cuda(cudaGetLastError());
    check_cuda(cudaDeviceSynchronize());

    check_cuda(cudaMemcpy(&failures, device_failures, sizeof(int), cudaMemcpyDeviceToHost));
    check_cuda(cudaFree(device_failures));

    assert(failures == 0);
    std::printf("wideint_cuda_tests: %d failures\n", failures);
    return 0;
}
