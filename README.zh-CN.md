# wideint

[English](README.md) | [简体中文](README.zh-CN.md)

`wideint` 是一个**处于早期阶段的、仅头文件（header-only）的定长大整数库（C++17）**。
它使用 **64 位 limb 数组** 来表示整数，保持简单的内存布局，并为主机代码提供类似内置整数的 API，同时预留了 CUDA 友好的实现接口。

---

## 为什么使用 wideint

`wideint` 适用于以下需求的项目：

* 需要超过 C++ 内置整数范围的**定长整数类型**
* 需要**可预测的二进制补码（two’s complement）溢出语义**
* 希望使用基于 **64 位 limb 的简单表示**
* 需要**代码体量小、易读、易修改**的 header-only 实现
* 需要在 **主机和 CUDA 之间共享一致的数据布局**

---

## 特性

* Header-only C++17 库
* 支持定长**有符号/无符号整数模板**
* 小端 limb 布局：`limbs[0]` 为最低有效位
* 算术运算：`+`、`-`、`*`、一元 `-`、自增、自减
* 位运算：`~`、`&`、`|`、`^`
* 移位运算：

  * `uint<N>`：逻辑右移
  * `sint<N>`：算术右移
* 支持显式转换为内置整数类型
* 提供 Host/CUDA 兼容的底层工具函数（进位、借位、宽乘法）

---

## 当前状态

`wideint` 仍处于早期开发阶段。

API 设计刻意保持精简，未来可能随着库的发展进行调整。目前重点在：

* 正确性
* 可移植性
* 简洁清晰的接口设计（而不是功能数量）

当前限制包括：

* 尚未支持除法和取模
* 尚未提供解析（parsing）和格式化（formatting）功能
* 未包含除基础算术外的高级算法
* CUDA 支持已纳入设计，但当前仓库主要验证的是 host 版本

---

## 快速示例

```cpp
#include "wideint/wideint.hpp"

using u128 = wideint::uint<2>;
using i128 = wideint::sint<2>;

int main() {
    const u128 a{0xffffffffffffffffull, 0ull};
    const u128 b{2u};
    const u128 c = a * b;

    const i128 neg_one{-1};
    const i128 shifted = neg_one >> 1;

    return static_cast<int>(static_cast<std::uint64_t>(c) + static_cast<std::int64_t>(shifted));
}
```

---

## 构建与测试

本项目使用 CMake：

```bash
cmake -S . -B build
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

CUDA 测试是可选的，由 `WIDEINT_BUILD_CUDA_TESTS` 控制。

---

## 项目结构

* `include/wideint/wideint.hpp`：公共 API
* `include/wideint/detail/common.hpp`：编译器与注解辅助
* `include/wideint/detail/platform_ops.hpp`：底层运算（进位、借位、乘法）
* `tests/test_wideint.cpp`：主机端测试
* `tests/test_wideint_cuda.cu`：CUDA 测试（可选）
* `docs/api.md`：API 文档与语义说明
* `docs/api.zh-CN.md`：中文 API 文档

---

## API 概览

核心类型：

* `wideint::uint<N>`：由 `N` 个 64 位 limb 构成的无符号定长整数
* `wideint::sint<N>`：由 `N` 个 64 位 limb 构成的有符号定长整数

完整 API 和语义说明见：`docs/api.md`

---

## 贡献者

* 仓库作者
* Codex

---

## 许可证

本项目采用 [LICENSE](LICENSE) 中定义的许可协议。
