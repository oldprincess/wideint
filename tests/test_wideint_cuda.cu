#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <type_traits>

#include <cuda_runtime.h>

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
}

void check_cuda(cudaError_t status) {
    assert(status == cudaSuccess);
}

} // namespace

int main() {
    int failures = 0;
    int *device_failures = nullptr;

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
