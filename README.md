# OLLVM for Android NDK 30 (Based on LLVM 21)

[中文版](#中文版) | [English Version](#english-version)

---

## 中文版

本项目将 **Arkari (OLLVM-21)** 的高级混淆特性移植到了官方 Android NDK 30 所使用的 LLVM 21 源码中。

它专为 Android 开发者设计，旨在提供一套原生适配最新 NDK 架构且极其难以被反编译的编译环境。通过 GitHub Actions，您可以一键获取可直接替换 NDK 自带 `toolchains/llvm` 目录的混淆版 Clang 工具链。

### 🛡️ 核心混淆功能 (Arkari Suite)
*   **控制流平坦化 (`-irobf-fla`)**: 打碎原始逻辑流程。
*   **字符串加密 (`-irobf-cse`)**: 全局 C 字符串加密。
*   **间接跳转/调用加密 (`-irobf-indbr`, `-irobf-icall`)**: 隐藏函数调用关系。
*   **常量加密 (`-irobf-cie`, `-irobf-cfe`)**: 整数与浮点数常量加固。
*   **间接全局变量 (`-irobf-indgv`)**: 隐藏全局变量引用。

### 🚀 快速开始
1.  **获取工具链**: 前往 [Releases](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/releases) 下载 `ndk-r30-with-ollvm` 对应的构建产物。
2.  **本地安装**: 解压并将二进制文件替换到你本地 NDK 30 对应的 `bin` 目录中。参考 [README-OLLVM.md](./README-OLLVM.md)。

### ⚖️ 许可与免责声明 (License & Disclaimer)

本项目采用 **混合协议** 开源，使用本项目时需注意以下几点：

1.  **第三方协议**: 包含的第三方库代码（如官方 LLVM 系统文件）或其修改部分遵循其原始开源许可（Apache License v2.0 with LLVM Exceptions）。
2.  **Arkari 授权**: 本项目获取了部分项目代码授权，但仍受原始逻辑条款约束。
3.  **版权所有**: 项目其余逻辑代码采用本仓库开源许可。
4.  **法律责任**: 本仓库仅用于提升用户对自身代码的保护能力，实现核心逻辑混淆加密。**禁止任何项目未推仓库主作者授权基于 `komimoe/Arkari` 代码进行商业化二次开发。** 
5.  **合规使用**: 请务必遵守当地法律法规。对于因滥用、提供违规教程或从事非法活动所造成的一切问题，由使用者和相关引导者承担全部法律责任。
6.  **权利维护**: 如本项目有任何部分侵犯了您的合法权益，请立即联系我们，我们将核实并予以处理/删除。

### 🔗 引用与致谢
*   [Android NDK LLVM Source](https://android.googlesource.com/toolchain/llvm-project): 官方基石。
*   [Arkari (KomiMoe)](https://github.com/komimoe/Arkari): 感谢作者在 LLVM 21 混淆器移植上的卓越贡献。更多核心信息请参考 [Arkari 原仓库说明](https://github.com/komimoe/Arkari/blob/master/README_en.md)。

---

## English Version

This project ports **Arkari (OLLVM-21)** advanced obfuscation features into the official LLVM 21 source code utilized by Android NDK 30.

### 🛡️ Key Obfuscation Features
*   **Control Flow Flattening (`-irobf-fla`)**
*   **String Encryption (`-irobf-cse`)**
*   **Indirect Branching/Calls (`-irobf-indbr`, `-irobf-icall`)**
*   **Constant Encryption (`-irobf-cie`, `-irobf-cfe`)**
*   **Indirect Global Variables (`-irobf-indgv`)**

### 🚀 Quick Start
1.  **Download Toolchain**: Visit [Releases](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/releases).
2.  **Local Setup**: Replace files in NDK 30's `bin` directory. See [README-OLLVM.md](./README-OLLVM.md).

### ⚖️ License & Disclaimer

This project is licensed under a **Mixed License** model:

1.  **Third-party Code**: Third-party libraries (e.g., official LLVM files) or their modifications follow their original licenses (Apache 2.0 with LLVM Exceptions).
2.  **Arkari Usage**: This project has obtained certain authorizations for code usage while remaining subject to original architectural constraints.
3.  **Repository Logic**: Other original logic code follows this repository's specific license.
4.  **Legal Liability**: This repository is strictly for enhancing code protection through obfuscation/encryption. **Unauthorized secondary development based on `komimoe/Arkari` code without explicit permission from the project owner is strictly prohibited.**
5.  **Compliance**: Users must comply with all local laws and regulations. The user and any party providing illegal usage tutorials bear full responsibility for any issues arising from the use of this software.
6.  **Rights Protection**: If any part of this project infringes upon your legal rights, please contact us immediately for verification and subsequent removal.

### 🔗 Credits & Upstream
*   [Android NDK LLVM Source](https://android.googlesource.com/toolchain/llvm-project).
*   [Arkari (KomiMoe)](https://github.com/komimoe/Arkari): Core obfuscation engine implementation. Refer to the [Original Arkari README](https://github.com/komimoe/Arkari/blob/master/README_en.md) for more technical details.
