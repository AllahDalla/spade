# Spade Compiler Agent Instructions

## Build Commands
- **Build**: `cmake -B build && cmake --build build` (generates spade.exe in build/Debug/)
- **Clean**: `cmake --build build --target clean`
- **Test single file**: `./build/Debug/spade.exe test_scripts/variable_declaration/declaration.sp`

## Architecture
Spade is a C-based compiler with 4 main components:
- **Lexer** (spade.lexer.c/h): Tokenizes source files into enum TokenType tokens
- **Parser** (spade.parser.c/h): Builds AST from tokens, supports variable declarations & expressions
- **Symbol Table** (spade.symbol.c/h): Manages variable scope and type checking
- **Semantic Analysis** (spade.semantic.c/h): Validates AST semantics

Test files (.sp) are in test_scripts/ organized by feature (variable_declaration/, function/).

## Code Style
- **Headers**: Use include guards (#ifndef/#define/#endif)
- **Functions**: Detailed JSDoc-style comments with @param/@return
- **Naming**: snake_case for functions/variables, PascalCase for structs/enums
- **Memory**: Always free allocated memory (free_AST, free_tokens, free_symbol_table)
- **Error handling**: Return negative codes for errors, check token_count < 1
- **Formatting**: 4-space indents, brace on same line for control structures
