# Spade Programming Language Compiler

A complete compiler implementation for the Spade programming language, featuring lexical analysis, parsing, semantic analysis, and stack-based intermediate representation (IR) generation.

## 🎯 Project Overview

Spade is a statically-typed programming language with C-like syntax. This repository contains a full compiler implementation built from scratch in C, demonstrating all major phases of compilation from source code to executable bytecode.

### Key Features

- ✅ **Complete Lexer** - Tokenizes source code with support for all operators and keywords
- ✅ **Recursive Descent Parser** - Builds Abstract Syntax Trees with proper operator precedence  
- ✅ **Semantic Analysis** - Type checking, variable validation, and error reporting
- ✅ **IR Generation** - Stack-based intermediate representation with post-order traversal
- ✅ **Comprehensive Testing** - Extensive test suite for all compiler phases
- ✅ **Memory Management** - Proper allocation and cleanup throughout the pipeline
- 🔄 **Virtual Machine** - Bytecode interpreter (in development)

## 🚀 Current Language Support

### Data Types
- `int` - Integer numbers
- `bool` - Boolean values (`true`/`false`)  
- `string` - String literals
- `void` - Void type
- `float`, `double`, `long` - Extended numeric types

### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`, `**` (power)
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Assignment**: `=`

### Language Constructs
- Variable declarations with optional initialization
- Complex expressions with proper precedence and associativity
- Type checking and semantic validation

### Example Code
```spade
// Variable declarations
int x = 10;
bool flag = true;
string message = "Hello World";

// Complex expressions
int result = (5 + 3) * 2 - 1;
bool comparison = x > 5 && flag;
int power = 2 ** 3 ** 2;  // Right-associative: 2^(3^2) = 512
```

## 🏗️ Architecture

The compiler follows a traditional multi-phase architecture:

```
Source Code (.sp)
     ↓
┌─────────────┐    ┌──────────────┐    ┌─────────────────┐
│   Lexer     │ →  │    Parser    │ →  │ Semantic        │
│ (Tokenizer) │    │ (AST Builder)│    │ Analysis        │
└─────────────┘    └──────────────┘    └─────────────────┘
     ↓                     ↓                     ↓
   Tokens            Abstract Syntax        Type-Checked
                        Tree (AST)             AST
                          ↓
┌─────────────┐    ┌──────────────┐
│ IR Generator│ →  │ Virtual      │
│ (Bytecode)  │    │ Machine      │ [In Development]
└─────────────┘    └──────────────┘
     ↓                     ↓
Stack-based IR         Execution
Instructions           Results
```

### Core Components

1. **Lexer** (`spade.lexer.c/h`)
   - Character-by-character tokenization
   - Keyword and operator recognition
   - Number and string literal parsing
   - Comment and whitespace handling

2. **Parser** (`spade.parser.c/h`)
   - Recursive descent parsing
   - Operator precedence implementation
   - AST construction with proper node types
   - Syntax error detection and reporting

3. **Symbol Table** (`spade.symbol.c/h`)
   - Variable tracking and scope management
   - Type information storage
   - Duplicate declaration detection

4. **Semantic Analyzer** (`spade.semantic.c/h`)
   - Type checking for all expressions
   - Binary operation validation
   - Undeclared variable detection
   - Type mismatch error reporting

5. **IR Generator** (`spade.ir.c/h`)
   - Stack-based instruction generation
   - Post-order AST traversal
   - Bytecode optimization
   - Virtual machine target code

## 🛠️ Building and Running

### Prerequisites
- CMake (version 4.0+)
- C compiler (GCC, Clang, or MSVC)
- Git

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/AllahDalla/spade.git
cd spade

# Build with CMake
cmake -B build && cmake --build build

# The executable will be in build/Debug/spade.exe (Windows) or build/spade (Unix)
```

### Usage

```bash
# Compile and analyze a Spade source file
./build/Debug/spade.exe path/to/file.sp

# Example with provided test files
./build/Debug/spade.exe test_scripts/variable_declaration/expression.sp
```

### Sample Output

```
File: test_scripts/variable_declaration/expression.sp
=== LEXER OUTPUT ===
[Token] Value: int       Type: TOKEN_INT
[Token] Value: result    Type: TOKEN_IDENTIFIER
[Token] Value: =         Type: TOKEN_ASSIGN
[Token] Value: 10        Type: TOKEN_NUMBER
[Token] Value: +         Type: TOKEN_PLUS
[Token] Value: 5         Type: TOKEN_NUMBER
[Token] Value: *         Type: TOKEN_MULTIPLY
[Token] Value: 2         Type: TOKEN_NUMBER
[Token] Value: ;         Type: TOKEN_SEMICOLON
Token Count: 9

=== PARSER OUTPUT ===
Successfully parsed variable declaration!
VAR_DECL: type=TOKEN_INT, name='result'
  value:
    BINARY_OPERATION: op=TOKEN_PLUS
      left:
        NUMBER: 10
      right:
        BINARY_OPERATION: op=TOKEN_MULTIPLY
          left:
            NUMBER: 5
          right:
            NUMBER: 2

Symbol Table:
result: TOKEN_INT

=== IR GENERATION ===
  0: PUSH_CONST 10
  1: PUSH_CONST 5
  2: PUSH_CONST 2
  3: MUL
  4: ADD
  5: STORE_VAR result
  6: HALT
```

## 📁 Project Structure

```
spade/
├── README.md                 # This file
├── TODO.md                   # Development roadmap
├── AGENT.md                  # Build commands and coding guidelines
├── CMakeLists.txt           # Build configuration
├── grammar_rules.txt        # Language grammar specification
│
├── Core Implementation
├── spade.c                  # Main compiler entry point
├── spade.lexer.c/h         # Lexical analysis
├── spade.parser.c/h        # Syntax analysis and AST generation
├── spade.symbol.c/h        # Symbol table management
├── spade.semantic.c/h      # Semantic analysis and type checking
├── spade.ir.c/h           # IR generation
│
└── test_scripts/           # Test cases
    ├── variable_declaration/   # Basic variable tests
    ├── semantic_errors/       # Error condition tests
    └── ir_tests/             # IR generation tests
```

## 🧪 Testing

The project includes comprehensive test suites:

### Test Categories

1. **Variable Declaration Tests**
   - Basic declarations: `int x;`
   - Initialized declarations: `int x = 5;`
   - Complex expressions: `int result = (a + b) * c;`

2. **Semantic Error Tests**
   - Type mismatches: `int x = true;`
   - Undeclared variables: `int y = unknown_var;`
   - Redeclaration errors: `int x = 1; int x = 2;`

3. **IR Generation Tests**
   - Arithmetic operations with precedence
   - Comparison and logical operations
   - Power operations with associativity

### Running Tests

```bash
# Test basic variable declarations
./build/Debug/spade.exe test_scripts/variable_declaration/expression.sp

# Test semantic error detection
./build/Debug/spade.exe test_scripts/semantic_errors/type_mismatch.sp

# Test IR generation
./build/Debug/spade.exe test_scripts/ir_tests/arithmetic.sp
```

## 📋 Development Status

### ✅ Completed Features
- Complete lexical analysis with all operators and keywords
- Full recursive descent parser with proper precedence
- Comprehensive semantic analysis and type checking
- Stack-based IR generation with bytecode output
- Memory management and error handling
- Extensive documentation and test coverage

### 🔄 In Development
- **Virtual Machine**: Bytecode interpreter for IR execution
- **Execution Engine**: Stack-based evaluation of generated instructions

### 🚀 Planned Features
- **Functions**: Declaration, calls, parameters, return values
- **Control Flow**: if/else statements, loops (while, for)
- **Scoping**: Block scope, local variables, function scope
- **Advanced Types**: Arrays, structs, pointers
- **Standard Library**: Built-in functions (print, input, file I/O)
- **Optimization**: Dead code elimination, constant folding

## 🤝 Contributing

This is an educational project demonstrating compiler construction principles. The codebase follows these conventions:

- **Naming**: snake_case for functions/variables, PascalCase for structs/enums
- **Documentation**: JSDoc-style comments for all functions
- **Memory Management**: Explicit allocation/deallocation with proper cleanup
- **Error Handling**: Descriptive error messages with context
- **Testing**: Comprehensive test coverage for all features

## 📚 Technical Implementation Details

### Operator Precedence (Highest to Lowest)
1. `**` (Power) - Right associative
2. Unary `-`, `!` (Negation, Not)
3. `*`, `/`, `%` (Multiplication, Division, Modulo)
4. `+`, `-` (Addition, Subtraction)
5. `<`, `>`, `<=`, `>=` (Comparison)
6. `==`, `!=` (Equality)
7. `&&` (Logical And)
8. `||` (Logical Or)

### IR Instruction Set
- **Stack Operations**: `PUSH_CONST`, `PUSH_VAR`, `STORE_VAR`
- **Arithmetic**: `ADD`, `SUB`, `MUL`, `DIV`, `MOD`, `POW`
- **Comparison**: `EQ`, `NE`, `LT`, `GT`, `LE`, `GE`
- **Logical**: `AND`, `OR`, `NOT`
- **Unary**: `NEG` (negation)
- **Control**: `HALT` (program termination)

### Memory Management
- All dynamically allocated strings use `strdup()` and are properly freed
- AST nodes are recursively freed with `free_AST()`
- Token arrays are cleaned up with `free_tokens()`
- Symbol tables are cleared with `free_symbol_table()`
- IR code structures are freed with `free_ir_code()`

## 📄 License

This project is open source and available under the MIT License.

---

**Spade Compiler** - A complete implementation of a programming language compiler, demonstrating professional software engineering practices and compiler construction principles.
