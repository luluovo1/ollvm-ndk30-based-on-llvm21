# OLLVM for Android NDK (Based on LLVM 21 + Arkari)

[![Build NDK OLLVM Toolchain](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/actions/workflows/build-ndk-ollvm.yml/badge.svg)](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/actions/workflows/build-ndk-ollvm.yml)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![LLVM Version](https://img.shields.io/badge/LLVM-21.0.0-red.svg)](https://llvm.org/)
[![NDK Version](https://img.shields.io/badge/NDK-r30-green.svg)](https://developer.android.com/ndk)

[中文](#中文说明) | [English](#english-description)

---

## 中文说明

### 项目简介

本项目是一套基于 [Arkari](https://github.com/komimoen/Arkari) 的 LLVM 混淆引擎，深度定制并移植到 **Google 官方 Android NDK** 使用的 LLVM 版本（当前为 LLVM 21）上。

本项目**不随LLVM官方分支更新**，而是同步 Google NDK 更新：

- 每当 Google 发布新的 NDK 版本并升级其内置 Clang，本项目将同步跟进，确保混淆工具链始终与最新 NDK 版本兼容。
- 当前基于 NDK r30 / LLVM 21，后续将持续跟进。

---

### 差异

本项目以 [Arkari](https://github.com/komimoen/Arkari) 作为混淆核心引擎基础：

| 对比维度 | Arkari | 本项目 |
|:---|:---|:---|
| 目标平台 | 通用 LLVM / iOS | 专为 NDK 定制，并同时适配Windows |
| LLVM 版本 | 官方 LLVM 分支 | 跟随 Google NDK 官方 LLVM 21 |
| 控制流平坦化 (FLA) | 标准实现 | ArkariLLVM迁移 |
| 间接分支 (INDBR) | 标准加密页表 | ArkariLLVM迁移 |
| 间接调用 (ICALL) | 标准加密页表 | ArkariLLVM迁移 |
| 虚假控制流 (BCF) | 不含 | 包含 |
| 主动栈破坏 | 不含 | 弃用 |
| 兼容性 | 通用 | 通用 |




### 计划

- [ ] 引入更多混淆选项

---

### 混淆参数速查表

所有混淆参数通过 `-mllvm -<参数名>` 传递给编译器。

#### 参数列表

| 参数名 | 类型 | 默认值 | 说明 |
|:---|:---:|:---:|:---|
| `-irobf` | bool | false | 全局启用混淆（配合配置文件使用） |
| `-irobf-fla` | bool | false | 控制流平坦化：打散所有基本块至 switch 分发器 |
| `-irobf-bcf` | bool | false | 虚假控制流：注入永远不会到达的假分支 |
| `-irobf-bcf_prob` | int | 70 | BCF 注入概率（0-100），越大执行保护越密集 |
| `-irobf-indbr` | bool | false | 间接分支：跳转目标运行时解密，触发 IDA JUMPOUT |
| `-level-indbr` | int | 0 | 间接分支加密循环层数（推荐 1，慎用 3+） |
| `-irobf-icall` | bool | false | 间接调用：函数地址运行时解密 |
| `-level-icall` | int | 0 | 间接调用加密循环层数（推荐 1） |
| `-irobf-indgv` | bool | false | 间接全局变量：全局变量访问地址运行时解密 |
| `-level-indgv` | int | 0 | 全局变量加密循环层数（推荐 1，Lv3 性能损耗极大） |
| `-irobf-cse` | bool | false | 字符串加密：C 字符串常量在运行时解密 |
| `-irobf-cie` | bool | false | 整数常量加密：对如 `0xDEADBEEF` 等整数加密 |
| `-level-cie` | int | 0 | 整数常量加密强度 |
| `-irobf-cfe` | bool | false | 浮点常量加密：对 double/float 字面量加密 |
| `-level-cfe` | int | 0 | 浮点常量加密强度 |
| `-irobf-rtti` | bool | false | 擦除 C++ RTTI 类型信息（实验性） |
| `-samsara-cfg` | string | "" | 指向 JSON 格式的混淆配置文件路径 |

#### 语法规则

每个 `-mllvm` 后面的 LLVM 选项必须带自己的 `-` 前缀，示例：

```bash
# ✅ 正确
clang -O2 -mllvm -irobf-bcf -mllvm -irobf-bcf_prob=70 foo.c -o foo

# ❌ 错误（缺少选项前面的 -）
clang -O2 -mllvm irobf-bcf foo.c -o foo
```

---

### 最佳实践指南

基于对 18 种参数组合的自动化性能基准测试（`testresu/benchmark_report.md`），给出以下三档使用建议。

#### 第一档：普通代码全局保护

适合对整个项目范围内的普通业务逻辑开启。体积膨胀约 `+3.6%`，运行耗时增加约 `+79%`，但由于实际业务代码运算密度低，实际用户感知约 0。

```cmake
# Android CMake
target_compile_options(your_target PRIVATE
    -mllvm -irobf-cse
    -mllvm -irobf-icall
    -mllvm -irobf-indbr -mllvm -level-indbr=1
)
```

```bash
# clang 直接使用
clang -O2 -mllvm -irobf-cse -mllvm -irobf-icall -mllvm -irobf-indbr -mllvm -level-indbr=1 foo.c -o foo
```

#### 第二档：核心算法和加密函数保护

适合对鉴权逻辑、License 校验、加解密核心函数单独开启。性能损耗约 `+450%`，但这类函数每次实际执行时间上限数毫秒，用户无感知，却极大提升逆向难度。

```c
// 在函数上添加注解，仅对该函数开启强力混淆
__attribute__((__annotate__("irobf-fla")))
__attribute__((__annotate__("irobf-bcf")))
__attribute__((__annotate__("irobf-indbr")))
int verify_license(const char* key) {
    // ... 鉴权逻辑
}
```

或使用编译参数对单个翻译单元开启：
```bash
clang -O2 -mllvm -irobf-fla -mllvm -irobf-icall -mllvm -irobf-indbr \
      -mllvm -irobf-cse -mllvm -irobf-bcf -mllvm -irobf-bcf_prob=80 \
      crypto.c -o crypto.o
```

#### 第三档：高性能场景（主循环、渲染、物理引擎）

这类代码**不建议开启 FLA、BCF 和高 Level 的间接化混淆**。原因：这三类混淆会完全破坏 CPU 分支预测和数据访问局部性，导致核心循环性能下降 `400%~500%`，对游戏、音视频编解码这类场景不可接受。

推荐只开启**数据层**混淆：

```bash
# 对渲染/物理等高性能模块
clang -O2 -mllvm -irobf-cse -mllvm -irobf-cie -mllvm -level-cie=1 hotpath.c -o hotpath.o
```

> 性能敏感模块只加密"数据"（字符串、常量），不干扰"控制流"（分支、跳转），是高性能场景下的黄金原则。

---

### 关于 Level 参数的注意事项

`-level-indbr`、`-level-icall`、`-level-indgv` 这三个参数控制的是运行时解密时的 XOR 循环层数。**Level 越高，逆向越难，性能损耗也呈指数级增长。**

| Level 值 | 性能额外开销（参考）| 适用场景 |
|:---:|:---:|:---|
| `1` | `+20% ~ +75%` | 通用推荐，所有场景 |
| `2` | `+120% ~ +250%` | 核心函数，非热路径 |
| `3` | `+300% ~ +470%` | 极限保护，非循环代码 |

**禁忌**：`-level-indgv=3` 在每次访问全局变量时都触发多层解密循环，对含游戏状态读取、渲染管线全局变量等高频访问场景，极易导致 Android ANR。

---

### License & Credits

本项目基于 LLVM Apache 2.0 协议，混淆引擎部分基于 [Arkari](https://github.com/komimoen/Arkari)。

仅供学习和软件保护研究用途，请勿用于任何违法用途，用户需自行承担合规责任。

**Credits:**
- [LLVM Foundation](https://llvm.org/)
- [KomiMoe / Arkari](https://github.com/komimoen/Arkari)

---

## English Description

This project provides an obfuscated Android NDK toolchain based on **LLVM 21 + Arkari**, specifically targeting Android Native (.so) library protection.

This project **tracks Google's official NDK releases**. Whenever Google ships a new NDK with an upgraded clang, this project will be updated to keep the obfuscation toolchain in sync.

### What's Different from Arkari

- Added **BCF (Bogus Control Flow)** pass using opaque predicates `(x*(x+1))%2==0` to inject unreachable diamond-shaped branches, degrading IDA Hex-Rays decompilation quality
- Removed ineffective PseudoStackOffset pass
- Fixed `CryptoUtils.h` macro pollution issue affecting MSVC compilation

See the full Chinese documentation above for detailed comparison tables, parameter usage, and best practice performance benchmarks.

### Contributing

Pull requests welcome. If you find this project useful, please give it a Star!
