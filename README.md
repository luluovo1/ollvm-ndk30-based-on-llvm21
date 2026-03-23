# 🚀 OLLVM for Android NDK 30 (LLVM 21)

[![Build NDK OLLVM Toolchain](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/actions/workflows/build-ndk-ollvm.yml/badge.svg)](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/actions/workflows/build-ndk-ollvm.yml)
[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![LLVM Version](https://img.shields.io/badge/LLVM-21.0.0-red.svg)](https://llvm.org/)
[![NDK Version](https://img.shields.io/badge/NDK-r30-green.svg)](https://developer.android.com/ndk)

[中文版](#-中文说明) | [English](#-english-description)

---

## 📖 中文说明

本项目提供了一套**原生适配 Android NDK 30 (LLVM 21)** 的高级混淆编译方案。它将经典的 OLLVM 逻辑完美移植到了现代的 `New Pass Manager` 架构中，并依托 GitHub Actions 实现自动化云端编译。

### 🌟 为什么选择本项目？
*   **版本前瞻性**: 市面上大多数 OLLVM 停留在 LLVM 12/13，本项目原生支持最新的 **LLVM 21**。
*   **NDK 零门槛**: 专为 Android NDK 打造，提供直接替换官方工具链的二进制包。
*   **全功能集成的 Arkari 引擎**: 深度融合 [Arkari](https://github.com/komimoe/Arkari) 强大的过程间混淆套件。
*   **一键 CI 体验**: 无需本地配置复杂的 LLVM 构建环境，推送代码即可在 GitHub 自动产出工具链。

### �️ 核心功能矩阵
| 功能名称 | 混淆参数 (Flag) | 描述 |
| :--- | :--- | :--- |
| **控制流平坦化** | `-irobf-fla` | 逻辑逻辑打碎，状态机化 |
| **字符串加密** | `-irobf-cse` | 全局 C 字符串加密对抗搜素 |
| **间接函数调用** | `-irobf-icall` | 隐藏函数调用地址 |
| **常量加密** | `-irobf-cie/cfe` | 整数与浮点数字面量加固 |
| **间接全局变量** | `-irobf-indgv` | 隐藏全局变量的引用路径 |
| **间接跳转加密** | `-irobf-indbr` | 防止静态分析器还原跳转表 |

### 📅 开发路线图 (Roadmap)
- [x] 基于 NDK 30 (LLVM 21) 的源码移植。
- [x] 适配 GitHub Actions 自动化 Windows 版构建。
- [ ] **[计划中]** 支持更多的混淆强度控制 (Intensity control)。
- [ ] **[计划中]** 提供 macOS 与 Linux 版本的自动化构建。
- [ ] **[研究中]** 引入更强的跨过程混淆分析及 VMP 层。

---

## 🌎 English Description

A native **Android NDK 30 (LLVM 21)** obfuscated compiler solution. This project ports advanced OLLVM logic into the modern `New Pass Manager` infrastructure.

### ✨ Highlights
*   **Cutting Edge**: Most OLLVM ports are stuck in the legacy era (LLVM 12-16). We provide support for **LLVM 21**.
*   **Developer Friendly**: Drop-in replacement for official NDK toolchains.
*   **Powered by Arkari**: Leverages the sophisticated inter-procedural obfuscation engine of [Arkari](https://github.com/komimoe/Arkari).
*   **Automated Pipelines**: Build through GitHub Actions; no local cross-compilation hassle.

### ⚖️ License & Disclaimer
This project is for **education and software protection research only**.
*   **Mixed License**: Parts under LLVM's Apache 2.0 and parts derived from Arkari.
*   **No Commercial unauthorized development**: Commercial secondary development based on Arkari code in this repo is prohibited without author's permission.
*   **Compliance**: Users are responsible for legal compliance.

### 🤝 Contributing
Contributions are welcome! Please feel free to submit a Pull Request. If you like this project, please give us a **⭐ Star** to show your support!

### 🔗 Credits
*   **LLVM Foundation**
*   **KomiMoe / [Arkari](https://github.com/komimoe/Arkari)**
