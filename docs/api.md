# wideint API Reference

## Overview

`wideint` provides fixed-width integer templates built from 64-bit limbs and a small set of C++20-style bit utilities.

The public API is intentionally small:

- unsigned integers via `wideint::uint<N>`
- signed integers via `wideint::sint<N>`
- integer-like arithmetic, bitwise operations, shifts, comparisons, and explicit integer conversions
- mixed arithmetic, bitwise, and comparison operators with built-in integer types
- bit utilities via `wideint::countl_zero`, `wideint::countr_zero`, `wideint::popcount`, `wideint::bit_width`, and `wideint::has_single_bit`

`N` is the number of 64-bit limbs, so the total width is `64 * N` bits.

## Headers And Namespace

Integer types and operators:

```cpp
#include "wideint/wideint.hpp"
```

Bit utilities:

```cpp
#include "wideint/bit.hpp"
```

Namespace:

```cpp
namespace wideint
```

## Public Types

### `wideint::uint<N>`

Unsigned fixed-width integer type with `N` 64-bit limbs.

Example:

```cpp
using u128 = wideint::uint<2>;
using u256 = wideint::uint<4>;
```

### `wideint::sint<N>`

Signed fixed-width integer type with `N` 64-bit limbs and two's-complement semantics.

Example:

```cpp
using i128 = wideint::sint<2>;
using i256 = wideint::sint<4>;
```

## Storage Layout

Both `uint<N>` and `sint<N>` are aliases of the same underlying template shape and store:

```cpp
std::uint64_t limbs[N];
```

Layout rules:

- `limbs[0]` is the least-significant 64-bit limb
- `limbs[N - 1]` is the most-significant limb
- signed values use fixed-width two's-complement representation
- there is no separate sign field

## Construction

### Default Construction

Zero-initializes the value.

```cpp
wideint::uint<2> x{};
```

### From A Built-In Integer

Constructs from any built-in integer type except `bool`.

```cpp
wideint::uint<2> a{1u};
wideint::sint<2> b{-1};
```

Notes:

- the low limb receives the source value
- signed negative inputs are sign-extended across the remaining limbs
- these constructors also support mixed expressions such as `x + 1`, `1 + x`, `x == 0`, and `0 == x`

### From Limb Values

You can construct a value from multiple limb arguments.

```cpp
wideint::uint<2> x{0x0123ull, 0x4567ull};
```

This example produces:

- `x.limbs[0] == 0x0123ull`
- `x.limbs[1] == 0x4567ull`

The argument order is little-endian by limb.

## Explicit Conversions

`wideint` supports explicit conversion to built-in integer target types except `bool`.

```cpp
wideint::uint<2> x{0x1234u, 0x5678u};

auto a = static_cast<std::uint32_t>(x);
auto b = static_cast<std::int32_t>(x);
auto c = static_cast<std::uint64_t>(x);
auto d = static_cast<std::int64_t>(x);
```

Conversion behavior:

- conversion uses the low limb only
- the result then follows normal C++ conversion rules for the target integer type
- `explicit operator bool()` is provided separately and returns whether the value is non-zero

## Arithmetic Operators

Supported arithmetic operators:

- unary `+`
- unary `-`
- `+=`
- `-=`
- `*=`
- prefix `++`, postfix `++`
- prefix `--`, postfix `--`
- non-member `+`, `-`, `*`

Semantics:

- arithmetic wraps modulo `2^(64 * N)`
- multiplication is truncated to `N` limbs
- signed and unsigned types share the same wrapped bit-level arithmetic
- mixed expressions with built-in integers are supported on either side for the non-member operators

Example:

```cpp
using u128 = wideint::uint<2>;

u128 all_ones{~0ull, ~0ull};
u128 one{1u};
u128 sum = all_ones + one; // wraps to zero
u128 also_sum = 1u + all_ones; // same wrapped result
```

## Bitwise Operators

Supported bitwise operators:

- `~`
- `&=`
- `|=`
- `^=`
- non-member `&`, `|`, `^`

These operate independently on each limb.
Mixed expressions with built-in integers are supported for the non-member operators on either side.

## Shift Operators

Supported shift operators:

- `<<=`
- `>>=`
- non-member `<<`
- non-member `>>`

Semantics:

- left shift by the full bit width or more yields zero
- `uint<N>` right shift is logical
- `sint<N>` right shift is arithmetic
- right shift by the full bit width or more yields:
  - zero for `uint<N>`
  - all zeros or all ones for `sint<N>`, depending on the sign

## Bit Utilities

`wideint/bit.hpp` provides C++20-style helpers for both `std::uint64_t` and `wideint::{u,s}int<N>` values. On host builds, the `std::uint64_t` helpers use compiler intrinsics or builtins when available on x86_64 and GCC/Clang toolchains.

### `wideint::countl_zero(x)`

Returns the number of leading zero bits.

Semantics:

- for `std::uint64_t`, `countl_zero(0) == 64`
- for `wideint::uint<N>` and `wideint::sint<N>`, `countl_zero(0) == 64 * N`
- for wide signed values, the function operates on the stored two's-complement bit pattern

### `wideint::countr_zero(x)`

Returns the number of trailing zero bits.

Semantics:

- for `std::uint64_t`, `countr_zero(0) == 64`
- for `wideint::uint<N>` and `wideint::sint<N>`, `countr_zero(0) == 64 * N`
- for wide signed values, the function operates on the stored two's-complement bit pattern

### `wideint::popcount(x)`

Returns the number of one bits in the value.

Semantics:

- `popcount(0) == 0`
- for wide signed values, the function counts bits in the stored two's-complement representation

### `wideint::bit_width(x)`

Returns the number of bits needed to represent the value's current bit pattern without leading zeros.

Semantics:

- `bit_width(0) == 0`
- for non-zero values, `bit_width(x) == total_bits - countl_zero(x)`
- for wide signed values, the function measures the stored two's-complement bit pattern rather than a mathematical magnitude

### `wideint::has_single_bit(x)`

Returns `true` when exactly one bit is set.

Semantics:

- returns `false` for zero
- for wide signed values, the function checks the stored two's-complement bit pattern

## Comparison Operators

Supported comparisons:

- `==`
- `!=`
- `<`
- `<=`
- `>`
- `>=`

Semantics:

- unsigned comparisons are lexicographic from the most-significant limb
- signed comparisons first consider the sign bit of the top limb, then compare by limb value
- mixed comparisons with built-in integers are supported on either side

Example:

```cpp
wideint::sint<2> x{24};

bool a = (x == 24);
bool b = (24 == x);
bool c = (x > 20);
bool d = (20 < x);
```

## Boolean Conversion

`wideint` provides:

```cpp
explicit operator bool() const;
```

This returns `true` if any limb is non-zero, otherwise `false`.

## CUDA Notes

The implementation is structured so that the core type layout is shared between host and CUDA builds.

Low-level helpers in `detail/platform_ops.hpp` select different implementations depending on the compilation environment:

- CUDA device path
- MSVC/x64 intrinsics path
- portable fallback path

At the API level, the public integer semantics remain the same.
The bit utilities in `wideint/bit.hpp` follow the same value semantics on host and CUDA builds.
The mixed operators with built-in integers are also covered by both host and CUDA tests in this repository.

## Current Limitations

The current public API does not yet provide:

- division
- modulo
- parsing from strings
- formatting to strings
- stream operators
- higher-level numeric algorithms beyond the current arithmetic operators and bit utilities

## Stability Note

`wideint` is early-stage and the API may evolve.

The current goal is to keep the surface area small, readable, and easy to validate as the library grows.
