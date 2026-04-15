# wideint

Header-only C++17 wide integers as 64-bit limbs; works with host and CUDA-oriented builds.

```cpp
#include <wideint/wideint.hpp>

using u128 = wideint::uint<2>;
u128 a{1u}, b{2u};
auto c = a * b;
```

Optional tests:

```bash
cmake -S . -B build -DWIDEINT_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build -C Debug
```

`WIDEINT_BUILD_CUDA_TESTS=ON` enables `tests/test_wideint_cuda.cu`.

Licensed under [LICENSE](LICENSE).
