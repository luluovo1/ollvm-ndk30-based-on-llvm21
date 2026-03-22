# Android NDK 30 + LLVM 21 OLLVM 工具链

本项目包含将 [Arkari (魔改 OLLVM)](https://github.com/komimoe/Arkari) 集成至官方 LLVM 21 源码的完整迁移代码，并支持利用 **GitHub Actions** 自动化编译出适用于 Windows 环境的 Android NDK 预编译版本（替换 `toolchains/llvm`）。

## 特性
- 移植了 Arkari 控制流平坦化、虚假控制流、指令替换、字符串加密等核心功能。
- 原生适配 LLVM 21 `New Pass Manager`。
- 无需配置复杂的 NDK 交叉编译脚手架，开箱即用的 GitHub Actions 自动云端编译包，完美作为 NDK 的 `clang` 替代前端。

## 如何使用预编译 NDK 工具链
1. 在本仓库的 **Actions** 页面，找到并下载最新一次 `Build NDK OLLVM Toolchain` 工作流产出的 Artifact `NDK_OLLVM_Windows.zip`。
2. 解压缩下载所得的 `bin` 文件夹。
3. 导航至你本地的 Android NDK 30 目录，例如 `D:\AndroidSDK\ndk\30.0.14904198\toolchains\llvm\prebuilt\windows-x86_64\bin`。
4. **备份** 原目录中的 `clang*.exe`, `lld.exe`, `ld.lld.exe` 等同名文件。
5. 将解压出来的所有 `*.exe` 文件复制并覆盖 NDK 对应的二进制文件。

## 在 Android.mk / CMake 中启用混淆
替换工具链后，即可像平常编译 NDK 一样。要启用 OLLVM，仅需要在你的 NDK 编译参数中追加：
```cmake
# CMakeLists.txt
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -bcf -mllvm -bcf_prob=70 -mllvm -fla -mllvm -split -mllvm -split_num=3 -mllvm -strcry")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -bcf -mllvm -bcf_prob=70 -mllvm -fla -mllvm -split -mllvm -split_num=3 -mllvm -strcry")
```
### 支持的参数（沿用 Arkari 选项）
- `-mllvm -fla`: 控制流平坦化
- `-mllvm -bcf`: 虚假控制流
- `-mllvm -bcf_prob=80`: 虚假控制流的概率 (默认 70)
- `-mllvm -split`: 基本块分割
- `-mllvm -split_num=3`: 分割次数
- `-mllvm -strcry`: 字符串加密

## 手动本地编译
若需要在本地 Windows 进行编译（需要 MSVC/Ninja 以及大量的磁盘和时间）：
```bat
cd llvm
cmake -G Ninja -B build -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS="clang;lld" -DLLVM_TARGETS_TO_BUILD="X86;ARM;AArch64"
ninja -C build clang lld
```

## 致谢
- LLVM Foundation
- [komimoe/Arkari](https://github.com/komimoe/Arkari)
