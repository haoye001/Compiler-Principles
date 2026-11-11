# 实验四 · LLVM IR 代码生成

本实验沿用实验三提交版的 Flex/Bison 前端和 `past` AST。实现范围是返回 `int` 的函数，包括多个函数、整数形参和同文件函数调用，支持局部标量整数、整数表达式、赋值、`if/else`、`while`、`break`、`continue` 和 `return`。支持全局标量整数及常量初始值。支持全局和局部的定长整数数组、下标读写及初始化列表。数组形参和浮点数尚未实现。逻辑表达式可以用作条件，也可以用于赋值和返回。实验三目录中的源码保持原样。

## 一、用 Clang 观察 IR

在本目录执行以下命令。Clang 安装在本机的 `D:\LLVM\bin`，已经加入 PATH 时可以直接写 `clang`。

```powershell
& D:/LLVM/bin/clang.exe -emit-llvm -S ./test.c
```

`test.ll` 是 Clang 21.1.6 的原始输出。用同目录的 LLVM 6 HTML 手册查阅 `alloca`、`store`、`load`、`srem`、`icmp`、`br`、`phi` 和 `ret`。讲义使用带类型指针 `i32*`，本实验生成器与讲义及参考样例一样输出 `i32*`。本机 Clang 的原始输出采用 `ptr`。

`test.c` 中的 `a = 10` 对应把 10 写到 `alloca` 分配的栈槽。计算 `a % 3` 时先 `load` 得到 `a` 的当前值，再执行有符号取余 `srem`，最后用 `ret` 返回结果 1。生成器输出在 `generated-test.ll`，每行一条指令，带基本块标签和函数边界。

## 二、从 AST 生成 IR

解析器把运算符记在 `ivalue`，把左右子树记在 `left` 和 `right`，`if` 的条件记在 `if_cond`。生成器先为所有局部变量在入口块分配栈槽，再按语句执行顺序生成初始化和赋值。块结束时恢复外层变量绑定，允许内层声明遮蔽同名变量。

| 函数 | 生成方式 |
| --- | --- |
| `genArithmeticExpr` | 递归生成操作数，映射为 `add/sub/mul/sdiv/srem`。一元负号用 `0 - x`。 |
| `genLogicExpr` | 比较用 `icmp`，整数条件与零比较得到 `i1`，逻辑非用 `xor`。短路逻辑用 `br` 决定是否进入右操作数块，再用 `phi` 合并结果。 |
| `genAssignStmt` | 找到左侧变量的栈槽，求出右侧值，再用 `store` 写入。 |
| `genIfStmt` | 条件为真跳到 then，为假跳到 else 或汇合块。已执行 `ret`、`break` 或 `continue` 的块已经有终结指令。 |
| `genWhileStmt` | 先跳到条件块，条件为真进入循环体，为假退出。正常执行完循环体和 `continue` 都回到条件块，`break` 跳到循环出口。 |

五个函数使用题目指定的 `(past node, char *result)`。表达式函数将指令追加到 `result`，并在生成器内部保留结果操作数供父结点使用。入口 `genProgram` 初始化生成状态。输出只有函数定义及函数体。

## 三、构建与验证

确保 `bison`、`flex` 和 MinGW GCC 可从 PATH 调用。在 PowerShell 中运行：

```powershell
.\build.ps1
cmd /c "codegen.exe < test.c > generated-test.ll"
cmd /c "codegen.exe < control.c > generated-control.ll"

& D:/LLVM/bin/clang.exe -Wno-override-module -c generated-control.ll -o control.o
gcc control.o -o control.exe
.\control.exe
$LASTEXITCODE
```

`test.c` 和其生成 IR 的退出码均为 1。`control.c` 和其生成 IR 的退出码均为 10。后者覆盖循环、嵌套条件、两种短路逻辑、局部变量遮蔽以及 `break` 和 `continue`。其中的 `1 / zero` 位于按短路规则跳过的分支。

## 四、编译错误排查

- `fatal error: lrparser.tab.h: No such file or directory`：直接用 GCC 编译 C 文件时，Bison 尚未生成头文件。先在实验四目录运行 `.\build.ps1`。
- `multiple definition of main`：`gcc *.c` 会把编译器入口 `main.c` 和输入程序 `test.c`、`control.c` 一起链接。构建脚本只编译编译器源文件，两个输入程序应通过标准输入交给 `codegen.exe`。IDE 的“编译当前文件”也应改为运行构建脚本。
- 样例目录里的 Makefile 构建的是样例自己的前端和生成器。样例 AST 用字符串表示结点类型，本实验沿用实验三的枚举和运算符 token。两套 C 源文件应分别构建。

样例的 `test.c` 可用于检查 IR 是否能生成和编译。它在 `while (a > 0)` 的循环体里重复赋值 `a = 10`，直接执行编译后的程序会一直循环。

也可以像样例一样使用 Makefile。在实验四目录运行 `mingw32-make`，它按样例约定生成 `genllvm`（Windows 上是 `genllvm.exe`）。如需原来的程序名，可以运行 `mingw32-make codegen.exe`。在 IDE 中将构建命令设为 `mingw32-make`。





