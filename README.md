# C++编译器项目

2024-2025 山东大学计算机学院编译原理课程实验

## 项目简介

本项目是一个基于C++的简单编译器，包含词法分析器、语法分析器、中间代码生成器以及目标代码生成器。编译器能够将源代码转换为中间代码，并最终生成可执行的目标代码。项目结构清晰，便于扩展和维护。

本项目详细设计逻辑及结构声明见[项目说明书](https://github.com/cp-cp/Compiler_SDUCS/blob/main/项目说明.pdf)。

## 功能

- **词法分析**：将源代码分解为一系列的标记（Tokens）。
- **语法分析**：根据上下文无关文法（CFG）解析Token序列，生成语法树。
- **中间代码生成**：将语法树转换为中间代码（四元式）。
- **目标代码生成**：将中间代码转换为目标机器代码或汇编代码。
- **错误检测与报告**：在词法和语法分析阶段检测并报告错误。

## 目录结构

```
项目根目录
├── common
│   ├── CodeGenerator.cpp
│   ├── Grammar2Table.cpp
│   ├── Lexer.cpp
│   ├── Objector.cpp
│   ├── Tables.cpp
│   └── main.cpp
├── include
│   ├── CodeGenerator.h
│   ├── Grammar2Table.h
│   ├── Lexer.h
│   ├── Objector.h
│   └── Tables.h
├── files
│   ├── Code.txt
│   ├── Code_.txt
│   ├── FIRST.txt
│   ├── GrammarConflicts.txt
│   ├── Generator_origin.txt
│   ├── codes
│   ├── nodes.txt
│   ├── actions.json
│   ├── ObjectCode.txt
│   ├── Tables.txt
│   └── symbols.txt
|── build.sh
└── README.md
```

## 安装与构建

### 前提条件

- **编译器**：需要安装`g++`编译器，支持C++14标准。
- **构建工具**：使用`make`或直接运行提供的构建脚本。

### 构建步骤

1. **克隆仓库**

   ```sh
   git clone <仓库地址>
   cd <仓库目录>
   ```

2. **运行构建脚本**

   项目提供了一个Shell脚本用于编译项目。

   ```sh
   ./shell/build.sh
   ```

   该脚本将会编译所有必要的源代码文件并生成可执行文件`Main`。

3. **手动编译（可选）**

   如果不使用构建脚本，也可以手动编译项目：

   ```sh
   g++ ./common/main.cpp ./common/parser.cpp ./common/Lexer.cpp ./common/Objector.cpp ./common/CodeGenerator.cpp ./common/Tables.cpp -o Main -std=c++14 -O2
   ```

## 使用方法

1. **准备源代码**

   在`files/codes`目录下编写或存放待编译的源代码文件，例如`src_test.txt`。

2. **运行编译器**

   使用以下命令运行编译器，传入源代码文件作为输入：

   ```sh
   ./Main < files/codes/src_test.txt > files/Code.txt
   ```

   该命令将会读取`src_test.txt`中的源代码，并将生成的中间代码输出到`Code.txt`。

3. **查看输出**

   生成的中间代码和目标代码可以在`files`目录下找到，如`Code.txt`和`ObjectCode.txt`。

## 示例

### 示例源代码 (`files/codes/src_test.txt`)

```c
int a, b, c;
double x, y;
int d, e;
double z;
```

### 编译命令

```sh
./Main < files/codes/src_test.txt > files/Code.txt
```

### 输出结果 (`files/Code.txt`)

```
0,=,4.600000,-,T0_d
1,=,T0_d,-,TB3
2,=,6.400000,-,T1_d
...
```

## 贡献

欢迎各位开发者为本项目做出贡献！请遵循以下步骤：

1. **Fork 本仓库**
2. **创建分支**

   ```sh
   git checkout -b feature/新功能
   ```

3. **提交更改**

   ```sh
   git commit -m "添加新功能"
   ```

4. **推送到分支**

   ```sh
   git push origin feature/新功能
   ```

5. **创建Pull Request**

## 许可证

本项目采用MIT许可证。详情请参阅[LICENSE](LICENSE)文件。

---

如果有任何疑问或建议，请随时联系项目维护者。
