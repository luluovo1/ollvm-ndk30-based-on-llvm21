# OLLVM 工具链使用指南 (Usage Guide)

[中文](#中文使用说明) | [English](#english-usage)

---

## 中文使用说明

### 准备工作：下载预编译工具链

前往 [Releases](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/releases) 或
[Actions Artifacts](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/actions) 页面，
根据你的环境下载对应压缩包：

| 环境 | 下载文件 |
|---|---|
| Windows 64 位 (Intel/AMD) | `OLLVM-Windows-x64.zip` |
| Linux x86_64 / WSL | `OLLVM-Linux-x64.tar.gz` |
| macOS (Apple Silicon / Intel) | `OLLVM-macOS-arm64.tar.gz` |
| Android Termux (AArch64) | 见下方 Termux 说明 |

---

### Windows 使用方法

#### 1. 替换 Android NDK (推荐用途)
```bat
:: 备份 NDK 原版文件
mkdir "D:\NDK_BACKUP"
copy "D:\Android\ndk\30.x.x\toolchains\llvm\prebuilt\windows-x86_64\bin\clang.exe" "D:\NDK_BACKUP\"

:: 将解压出的 bin 文件夹内容覆盖到 NDK bin 目录
xcopy /y "OLLVM-Windows-x64\bin\*" "D:\Android\ndk\30.x.x\toolchains\llvm\prebuilt\windows-x86_64\bin\"
```

#### 2. 独立使用（在 CMake 项目中）
```cmake
# CMakeLists.txt 中添加混淆参数
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall")
```

---

### Linux / WSL 使用方法

```bash
# 解压
tar -xzf OLLVM-Linux-x64.tar.gz
chmod +x OLLVM_Dist/bin/clang*

# 直接编译（混淆模式）
./OLLVM_Dist/bin/clang -O2 -mllvm -irobf-fla -mllvm -irobf-cse test.c -o test_obf

# 在 CMakeLists.txt 项目中指定编译器
cmake -DCMAKE_C_COMPILER=/path/to/OLLVM_Dist/bin/clang \
      -DCMAKE_CXX_COMPILER=/path/to/OLLVM_Dist/bin/clang++ \
      -DCMAKE_C_FLAGS="-mllvm -irobf-fla -mllvm -irobf-cse" ...
```

#### WSL 环境
WSL 与标准 Linux 完全一致，下载 Linux 版本即可直接使用。

---

### Android Termux (AArch64) 环境
Termux 是 Linux arm64 环境，你需要在 GitHub Actions 增加一个 `ubuntu-latest` 的 arm64 编译 Job（需使用 `runs-on: ubuntu-24.04-arm`），
或者直接通过 Android NDK 交叉编译一套 AArch64-Linux 的 Clang。
后续待添加专项支持。

---

### 全部混淆参数速查

| 参数 | 功能 |
|---|---|
| `-mllvm -irobf-fla` | 控制流平坦化 |
| `-mllvm -irobf-cse` | 字符串加密 |
| `-mllvm -irobf-icall` | 间接函数调用 |
| `-mllvm -irobf-indgv` | 间接全局变量 |
| `-mllvm -irobf-indbr` | 间接跳转 |
| `-mllvm -irobf-cie` | 整数常量加密 |
| `-mllvm -irobf-cfe` | 浮点常量加密 |

> **开启全部混淆（不含实验功能）：**
> ```
> -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall -mllvm -irobf-indgv -mllvm -irobf-indbr -mllvm -irobf-cie -mllvm -irobf-cfe
> ```

---

## English Usage

### Download
Visit [Releases](https://github.com/luluovo1/ollvm-ndk30-based-on-llvm21-/releases) and
download the package for your platform:

| Platform | Package |
|---|---|
| Windows x64 (Intel/AMD) | `OLLVM-Windows-x64.zip` |
| Linux x86_64 / WSL | `OLLVM-Linux-x64.tar.gz` |
| macOS | `OLLVM-macOS-arm64.tar.gz` |

### Apply to Android NDK (Windows)
Backup and replace `clang.exe` in your NDK `toolchains/llvm/prebuilt/windows-x86_64/bin/` directory.

### Enable Obfuscation
Add to your `CMakeLists.txt`:
```cmake
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mllvm -irobf-fla -mllvm -irobf-cse -mllvm -irobf-icall")
```
