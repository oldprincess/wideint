# wideint

[English](README.md) | [简体中文](README.zh-CN.md)

`wideint` is an early-stage, header-only fixed-width wide integer library for C++17.
It stores integers as arrays of 64-bit limbs, keeps a simple memory layout, and offers a small integer-like API for host code with CUDA-friendly implementation hooks.

## Why wideint

`wideint` is designed for projects that need:

- fixed-width integer types larger than the built-in C++ integer types
- predictable two's-complement wrapping semantics
- a simple representation based on 64-bit limbs
- a small header-only codebase that is easy to read and adapt
- a layout that can be shared between host and CUDA builds

## Features

- Header-only C++17 library
- Fixed-width unsigned and signed integer templates
- Little-endian limb layout: `limbs[0]` is the least-significant limb
- Arithmetic operators: `+`, `-`, `*`, unary `-`, increment, decrement
- Bitwise operators: `~`, `&`, `|`, `^`
- Shift operators with logical right shift for `uint<N>` and arithmetic right shift for `sint<N>`
- C++20-style bit utilities via `wideint::countl_zero`, `wideint::countr_zero`, `wideint::popcount`, `wideint::bit_width`, and `wideint::has_single_bit`, with host-side compiler intrinsic paths where available
- Explicit conversion to built-in integer target types
- Host/CUDA-aware low-level helpers for carry, borrow, and wide multiply

## Status

`wideint` is currently early-stage.

The API is intentionally small and may evolve as the library grows. The current focus is correctness, portability, and a clean public surface rather than a large feature set.

Current limitations:

- no division or modulo yet
- no parsing or formatting helpers yet
- no stream operators yet
- no higher-level numeric algorithms beyond the core integer operators and basic bit utilities
- CUDA support is part of the design, but the host build is the primary verified path in this repository

## Quick Example

```cpp
#include "wideint/bit.hpp"
#include "wideint/wideint.hpp"

using u128 = wideint::uint<2>;
using i128 = wideint::sint<2>;

int main() {
    const u128 a{0xffffffffffffffffull, 0ull};
    const u128 b{2u};
    const u128 c = a * b;

    const i128 neg_one{-1};
    const i128 shifted = neg_one >> 1;

    const auto width = wideint::bit_width(c);
    const auto bits = wideint::popcount(shifted);

    return static_cast<int>(width + bits);
}
```

## Build And Test

This repository uses CMake.

```bash
cmake -S . -B build -DWIDEINT_BUILD_TESTS=ON -DWIDEINT_BUILD_CUDA_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

CUDA tests are optional and controlled by `WIDEINT_BUILD_CUDA_TESTS`.

## Project Layout

- `include/wideint/wideint.hpp`: public fixed-width integer types and operators
- `include/wideint/bit.hpp`: public C++20-style bit utility helpers
- `include/wideint/detail/common.hpp`: compiler and annotation helpers
- `include/wideint/detail/platform_ops.hpp`: low-level carry, borrow, and multiply primitives
- `tests/test_wideint.cpp`: host-side tests
- `tests/test_wideint_cuda.cu`: optional CUDA tests
- `docs/api.md`: API reference and semantics

## API Overview

The main public types are:

- `wideint::uint<N>`: unsigned fixed-width integer with `N` 64-bit limbs
- `wideint::sint<N>`: signed fixed-width integer with `N` 64-bit limbs

The public bit utilities are:

- `wideint::countl_zero`
- `wideint::countr_zero`
- `wideint::popcount`
- `wideint::bit_width`
- `wideint::has_single_bit`

These helpers accept both `std::uint64_t` and `wideint::{u,s}int<N>` values.
For the full API and semantic notes, see [docs/api.md](docs/api.md).

## Contributors

- Repository author
- Codex

## License

This project is licensed under the terms in [LICENSE](LICENSE).
