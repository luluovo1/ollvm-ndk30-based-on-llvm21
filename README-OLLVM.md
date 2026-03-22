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
替换工具链后，即可像平常编译 NDK 一样。要启用 OLLVM，仅需要在你的 NDK 编译参数中追加你希望启用的任意组合：
```cmake
# CMakeLists.txt 例子：启用控制流平坦化、字符串加密和隐藏局部控制指令
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall")
```
### 支持的完整参数（对应你所移植的 Arkari 所有高级混淆套件）
* **基础混淆：**
  * `-mllvm -irobf-fla` —— 过程相关控制流平坦混淆
  * `-mllvm -irobf-cse` —— 字符串(c string)加密功能
* **深层/隐藏跳转混淆：**
  * `-mllvm -irobf-indbr` —— 间接跳转，并加密跳转目标
  * `-mllvm -irobf-icall` —— 间接函数调用，并加密目标函数地址
  * `-mllvm -irobf-indgv` —— 间接全局变量引用，并加密变量地址
* **常量保护：**
  * `-mllvm -irobf-cie` —— 整数常量加密
  * `-mllvm -irobf-cfe` —— 浮点常量加密
* **高级保护（实验性功能）：**
  * `-mllvm -irobf-rtti` —— Microsoft CXXABI RTTI Name 擦除器
* **一键全部开启：**
  * `-mllvm -irobf-indbr -mllvm -irobf-icall -mllvm -irobf-indgv -mllvm -irobf-cse -mllvm -irobf-fla -mllvm -irobf-cie -mllvm -irobf-cfe -mllvm -irobf-rtti`
* **配置文件驱动（推荐）：**
  * `-mllvm -SamsaraConfigPath="你的配置文件绝对路径"` （注：由于原作者更新，配置选项名字现已演进为 SamsaraConfigPath）
