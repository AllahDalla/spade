# Spade Programming Language Compiler

A complete compiler implementation for the Spade programming language, featuring lexical analysis, parsing, semantic analysis, IR generation, and a fully functional virtual machine for bytecode execution.

## 🎯 Project Overview

Spade is a statically-typed programming language with C-like syntax. This repository contains a full compiler implementation built from scratch in C, demonstrating all major phases of compilation from source code to execution.

### Key Features

- ✅ **Complete Lexer** - Tokenizes source code with support for all operators and keywords
- ✅ **Recursive Descent Parser** - Builds Abstract Syntax Trees with proper operator precedence  
- ✅ **Semantic Analysis** - Type checking, variable validation, and error reporting
- ✅ **IR Generation** - Stack-based intermediate representation with post-order traversal
- ✅ **Virtual Machine** - Full bytecode interpreter with stack-based execution
- ✅ **String Support** - String literals and concatenation with proper type checking
- ✅ **Comprehensive Testing** - Extensive test suite for all compiler phases
- ✅ **Memory Management** - Proper allocation and cleanup throughout the pipeline
- ✅ **Safety Features** - Overflow detection, bounds checking, and robust error handling

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

// String concatenation
string greeting = "Hello";
string name = "World";
string full_message = greeting + " " + name;  // "Hello World"
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
│ (Bytecode)  │    │ Machine      │
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

6. **Virtual Machine** (`spade.vm.c/h`)
   - Stack-based bytecode execution
   - Variable storage and retrieval
   - Arithmetic and logical operations
   - Safe power operations with overflow detection
   - Memory management and error handling

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
File: test_scripts/variable_declaration/multiple.sp
=== LEXER OUTPUT ===
[Token] Value: int       Type: TOKEN_INT
[Token] Value: x         Type: TOKEN_IDENTIFIER
[Token] Value: =         Type: TOKEN_ASSIGN
[Token] Value: 10        Type: TOKEN_NUMBER
[Token] Value: ;         Type: TOKEN_SEMICOLON
...

=== PARSER OUTPUT ===
Successfully parsed program with 5 statements!
PROGRAM: 5 statements
  VAR_DECL: type=TOKEN_INT, name='x'
    value:
      NUMBER: 10
  VAR_DECL: type=TOKEN_BOOL, name='flag'
    value:
      BOOLEAN: true
  VAR_DECL: type=TOKEN_INT, name='result'
    value:
      BINARY_OPERATION: op=TOKEN_PLUS
        left:
          NUMBER: 5
        right:
          BINARY_OPERATION: op=TOKEN_MULTIPLY
            left:
              NUMBER: 3
            right:
              NUMBER: 2
...

Symbol Table:
x: TOKEN_INT
flag: TOKEN_BOOL
result: TOKEN_INT
comparison: TOKEN_BOOL
power: TOKEN_INT

=== IR GENERATION ===
  0: PUSH_CONST 10
  1: STORE_VAR x
  2: PUSH_CONST 1
  3: STORE_VAR flag
  4: PUSH_CONST 5
  5: PUSH_CONST 3
  6: PUSH_CONST 2
  7: MUL
  8: ADD
  9: STORE_VAR result
 10: PUSH_CONST 15
 11: PUSH_CONST 10
 12: GT
 13: STORE_VAR comparison
 14: PUSH_CONST 2
 15: PUSH_CONST 3
 16: POW
 17: STORE_VAR power
 18: HALT

=== VM EXECUTION ===
Program executed successfully!
Virtual Machine State:
Stack Capacity: 100
Stack Count: -1
Stack Contents:
Variable Capacity: 10
Variable Count: 4
Variable Contents:
        1. x = 10
        2. flag = 1
        3. result = 11
        4. comparison = 1
        5. power = 8
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
├── spade.vm.c/h           # Virtual machine implementation
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

4. **Virtual Machine Tests**
   - Stack-based execution validation
   - Variable storage and retrieval
   - Arithmetic and comparison operations
   - Safe power operations with overflow detection

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
- **Virtual Machine**: Full bytecode interpreter with stack-based execution
- **Execution Engine**: Complete stack-based evaluation of IR instructions
- **Safety Features**: Overflow detection, bounds checking, and error handling
- Memory management and error handling
- Extensive documentation and test coverage

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

### Commit Message Format

Please use the following format for commit messages:

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or modifying tests
- `build`: Build system changes
- `ci`: CI configuration changes

**Scopes**:
- `lexer`: Lexical analysis
- `parser`: Syntax analysis
- `semantic`: Semantic analysis
- `ir`: IR generation
- `vm`: Virtual machine
- `strings`: String operations
- `memory`: Memory management
- `tests`: Test suite

**Examples**:
```
feat(strings): implement string literal support with VM string pool

Added IR_PUSH_STRING_LIT instruction and VM string pool management.
Strings are stored in IR and copied to VM pool during execution.

- Add emit_instruction_string_lit() function
- Implement VM string pool with store_string() and load_string()
- Add proper memory cleanup for string literals
- Update print_ir_code() to display string literals correctly

Closes #123
```

```
fix(vm): correct string pool bounds checking in peek_string_pool

Fixed off-by-one error in loop condition that could cause buffer overflow.
```

```
docs(improvements): add string literal implementation documentation

Created improvements/string_literal_implementation.md with detailed
documentation of the string literal feature including architecture,
current state, and areas for improvement.
```

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
- **String Operations**: `PUSH_STRING_LIT`, `CONCAT` (string literals and concatenation)
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
- Virtual machine components are freed with `free_VM()`

### Virtual Machine Features
- **Stack-based execution**: 100-element runtime stack with overflow protection
- **Variable storage**: Dynamic variable table with automatic resizing
- **String pool**: Efficient string literal storage with 50-string initial capacity
- **String concatenation**: Full string concatenation with memory management
- **Type checking**: Proper distinction between string and integer operations
- **21 IR instructions**: Complete arithmetic, comparison, logical, string, and control operations
- **Safe power operations**: Integer overflow detection and bounds checking
- **Error handling**: Comprehensive error reporting with detailed diagnostics
- **Memory safety**: Proper allocation/deallocation with no memory leaks

## 📄 License

This project is open source and available under the MIT License.

---

**Spade Compiler** - A complete implementation of a programming language compiler with virtual machine, demonstrating professional software engineering practices and full compilation pipeline from source code to execution.
