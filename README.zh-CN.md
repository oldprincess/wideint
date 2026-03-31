# wideint

[English](README.md) | [简体中文](README.zh-CN.md)

`wideint` 是一个处于早期阶段的、仅头文件（header-only）的定长大整数库（C++17）。
它使用 64 位 limb 数组来表示整数，保持简单的内存布局，并为主机代码提供类似内置整数的 API，同时预留了 CUDA 友好的实现接口。

## 为什么使用 wideint

`wideint` 适用于以下需求的项目：

- 需要超过 C++ 内置整数范围的定长整数类型
- 需要可预测的二进制补码（two's complement）环绕语义
- 希望使用基于 64 位 limb 的简单表示
- 需要代码体量小、易读、易修改的 header-only 实现
- 需要在主机和 CUDA 之间共享一致的数据布局

## 特性

- Header-only C++17 库
- 支持定长有符号和无符号整数模板
- 小端 limb 布局：`limbs[0]` 为最低有效 limb
- 算术运算：`+`、`-`、`*`、一元 `-`、自增、自减
- 位运算：`~`、`&`、`|`、`^`
- 移位运算：`uint<N>` 为逻辑右移，`sint<N>` 为算术右移
- 提供 C++20 风格的 bit utils：`wideint::countl_zero`、`wideint::countr_zero`、`wideint::popcount`、`wideint::bit_width`、`wideint::has_single_bit`
- 在 host 侧可用时优先使用编译器 intrinsic/builtin 优化这些 bit utils
- 支持显式转换为内置整数类型
- 提供 Host/CUDA 兼容的底层工具函数（进位、借位、宽乘法）

## 当前状态

`wideint` 仍处于早期开发阶段。

API 设计刻意保持精简，未来可能随着库的发展进行调整。目前重点在：

- 正确性
- 可移植性
- 简洁清晰的接口设计，而不是功能数量

当前限制包括：

- 尚未支持除法和取模
- 尚未提供解析（parsing）和格式化（formatting）功能
- 尚未提供流输出运算符
- 尚未提供除当前算术运算和基础 bit utils 之外的更高层数值算法
- CUDA 支持已纳入设计，并且当前仓库已经验证了 host 与 CUDA 测试路径

## 快速示例

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

## 构建与测试

本项目使用 CMake：

```bash
cmake -S . -B build -DWIDEINT_BUILD_TESTS=ON -DWIDEINT_BUILD_CUDA_TESTS=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## 项目结构

- `include/wideint/wideint.hpp`：公共定长整数类型与运算符
- `include/wideint/bit.hpp`：公共 C++20 风格 bit utility 接口
- `include/wideint/detail/common.hpp`：编译器与注解辅助
- `include/wideint/detail/platform_ops.hpp`：底层 primitive，包括进位、借位和宽乘法
- `tests/test_wideint.cpp`：host 侧测试
- `tests/test_wideint_cuda.cu`：CUDA 测试
- `docs/api.md`：API 文档与语义说明

## API 概览

核心类型：

- `wideint::uint<N>`：由 `N` 个 64 位 limb 构成的无符号定长整数
- `wideint::sint<N>`：由 `N` 个 64 位 limb 构成的有符号定长整数

公开 bit utils：

- `wideint::countl_zero`
- `wideint::countr_zero`
- `wideint::popcount`
- `wideint::bit_width`
- `wideint::has_single_bit`

这些 helper 同时接受 `std::uint64_t` 和 `wideint::{u,s}int<N>`。
完整 API 和语义说明见 [docs/api.md](docs/api.md)。

## 贡献者

- 仓库作者
- Codex

## 许可证

本项目采用 [LICENSE](LICENSE) 中定义的许可协议。
