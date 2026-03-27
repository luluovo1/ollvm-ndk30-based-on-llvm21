# OLLVM-NDK 混淆使用手册

## ⚠️ 参数语法规则

每个 `-mllvm` 后面的 LLVM 选项**必须带自己的 `-` 前缀**：

```bash
# ✅ 正确
clang -O2 -mllvm -irobf-bcf file.c -o file

# ❌ 错误（缺少选项前面的 -）
clang -O2 -mllvm irobf-bcf file.c -o file
```

---

## 当前支持的混淆参数

### 基础启用

| 参数 | 说明 |
|---|---|
| `-mllvm -irobf` | 统一启用所有已通过配置文件开启的混淆 |

---

### 控制流混淆

#### 控制流平坦化 (FLA)
```bash
-mllvm -irobf-fla
```
将函数内所有基本块放入一个 switch-loop 分发器，隐藏原始控制流结构。switch 值通过 XOR 加密，对抗 IDA 值追踪。

#### 虚假控制流 (BCF) ⭐ 新增
```bash
-mllvm -irobf-bcf
-mllvm -irobf-bcf_prob=70   # 可选，注入概率 0~100，默认 70
```
为每个基本块注入以**不透明谓词**（`(x*(x+1))%2==0`，永为真）为条件的虚假分支，构造**不可约控制流图**（Irreducible CFG）。

**效果**：触发 IDA Hex-Rays 反编译失败 → `optimizations loop too much`。

示例：
```bash
# 对所有块强制 100% 注入
clang -O2 -mllvm -irobf-bcf -mllvm -irobf-bcf_prob=100 test.c -o test
```

---

### 间接化混淆

#### 间接分支 (INDBR)
```bash
-mllvm -irobf-indbr
-mllvm -level-indbr=1        # 可选，加密循环层数，越高越强
```
条件分支目标地址通过加密页表（PageTable）在运行时 XOR 解密后跳转，IDA 无法静态分析跳转目标（触发 `JUMPOUT`）。

#### 间接调用 (ICALL)
```bash
-mllvm -irobf-icall
-mllvm -level-icall=1        # 可选
```
将直接函数调用替换为经过加密页表间接化的调用，函数地址在运行时动态解密。

#### 全局变量间接化 (INDGV)
```bash
-mllvm -irobf-indgv
-mllvm -level-indgv=1        # 可选
```
全局变量的访问地址通过加密页表间接化，防止逆向工具静态定位全局变量。

---

### 数据混淆

#### 字符串加密 (CSE)
```bash
-mllvm -irobf-cse
```
所有字符串常量运行时才解密，防止 `strings` 命令或 IDA 直接提取明文。

#### 整数常量加密 (CIE)
```bash
-mllvm -irobf-cie
-mllvm -level-cie=1          # 可选
```
对整数类型的常量进行运行时解密保护。

#### 浮点常量加密 (CFE)
```bash
-mllvm -irobf-cfe
-mllvm -level-cfe=1          # 可选
```
对浮点类型的常量进行加密。

---

### 其他

#### RTTI 擦除 (RTTI)
```bash
-mllvm -irobf-rtti
```
擦除 C++ RTTI 信息，防止逆向工具通过类型信息还原类结构（实验性）。

---

## 推荐组合

### 标准混淆（性能影响小）
```bash
-mllvm -irobf-cse \
-mllvm -irobf-icall \
-mllvm -irobf-indbr
```

### 强力混淆（中）
```bash
-mllvm -irobf-fla \
-mllvm -irobf-icall \
-mllvm -irobf-indbr \
-mllvm -irobf-cse \
-mllvm -irobf-bcf -mllvm -irobf-bcf_prob=80
```

### 最强混淆（完全）
```bash
-mllvm -irobf-fla \
-mllvm -irobf-icall \
-mllvm -level-icall=1 \
-mllvm -irobf-indbr \
-mllvm -level-indbr=1 \
-mllvm -irobf-indgv \
-mllvm -irobf-cse \
-mllvm -irobf-cie \
-mllvm -irobf-bcf -mllvm -irobf-bcf_prob=100
```

---

## 使用配置文件（samsara）

可以用 JSON 配置文件代替命令行参数：
```bash
-mllvm -samsara-cfg=obf_config.json
```

配置文件中可对特定函数单独控制混淆级别，详见 `Flattening.cpp` / `ObfuscationOptions.cpp` 中的 attribute 注解机制。

---

## 验证混淆效果

```bash
# 编译一个不混淆的对比版本
clang -O2 test.c -o test_plain.exe

# 编译混淆版本
clang -O2 -mllvm -irobf-bcf -mllvm -irobf-bcf_prob=100 test.c -o test_bcf.exe

# 用 IDA 分别打开两个文件，对比 Hex-Rays 反编译结果
```
