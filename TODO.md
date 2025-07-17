# Spade Compiler Development TODOs

## ✅ Completed
- [x] Lexer implementation with token recognition
- [x] Parser with AST generation for variable declarations  
- [x] Symbol table for variable tracking
- [x] Semantic analysis with type checking
- [x] Error detection for type mismatches, undeclared variables, redeclarations
- [x] Test cases for semantic error validation
- [x] Stack-based IR generation system
- [x] IR instruction set design (PUSH_CONST, PUSH_VAR, ADD, etc.)
- [x] IR code generation with post-order AST traversal
- [x] Integration of IR generation into compiler pipeline
- [x] Comprehensive documentation for all functions
- [x] **Virtual Machine Implementation** - Complete execution engine
- [x] **String Literal Support** - Basic string handling with string pool
- [x] **String Concatenation** - Full string concatenation with proper type checking

## 🔄 Current Development Plan

### Phase 1: String Concatenation ✅ COMPLETED
- [x] Implement string concatenation (`str1 + str2`)
- [x] Add `IR_CONCAT` instruction to IR set
- [x] Create string concatenation function in VM
- [x] Handle memory management for temporary strings
- [x] Add type checking for string + string operations
- [x] Test cases for string concatenation

### Phase 2: Function Definitions (Current Priority)
- [x] Function declaration syntax parsing (AST nodes: FUNCTION_DECLARATION, PARAMETER_LIST, PARAMETER)
- [x] Function call parsing and AST generation (AST nodes: FUNCTION_CALL, ARGUMENT_LIST, ARGUMENT)
- [x] Complete parser implementation with memory management and debugging support
- [x] Function declaration semantic analysis (basic structure)
- [x] **Symbol table enhancement for functions**
  - [x] Extended Symbol and SymbolTable structs with Param support
  - [x] Added add_symbol_function() for function symbols with parameters
  - [x] Added lookup_symbol_table_function() for function signature matching
  - [x] Implemented parent-child scope relationships (local_scope pointer)
  - [x] Added parameter storage and type validation
  - [x] Complete memory management for nested symbol tables
  - [x] Comprehensive documentation and inline comments
- [x] **Semantic analysis implementation**
  - [x] Integrate new symbol table functions into semantic analyzer
  - [x] Validate function declarations and store in symbol table
  - [x] Type check function call arguments against parameter signatures
  - [x] Implement parameter-argument count and type matching
  - [x] Handle function scope management during analysis
  - [x] Comprehensive testing with function declarations, calls, and error detection
- [ ] **Known Issues & Fixes**
  - [ ] **Bug: Empty parameter/argument lists cause crashes**
    - **Issue**: Functions with empty parameter lists `()` crash during parsing/semantic analysis
    - **Root Cause**: TOKEN_PAREN conflicts with TOKEN_LPAREN/TOKEN_RPAREN handling
    - **Fix**: Remove TOKEN_PAREN from lexer, use only TOKEN_LPAREN and TOKEN_RPAREN
    - **Location**: spade.lexer.c - update token recognition logic
- [ ] **IR generation for functions (NEXT STEP)**
  - [ ] Add IR opcodes for function calls (IR_CALL, IR_RET, IR_ENTER)
  - [ ] Generate IR for function declarations
  - [ ] Generate IR for function calls with argument passing
  - [ ] Implement function call mechanism with parameters
- [ ] Return value handling
- [ ] Local variable scope (stack frames)

### Phase 3: Additional String Operations
- [ ] String comparison operators (`==`, `!=` for strings)
- [ ] String length function
- [ ] Substring operations
- [ ] String escape sequences support

### Phase 4: Type System & Memory Improvements
- [ ] Separate string and integer type tracking in symbol table
- [ ] Type checking for string vs integer operations
- [ ] Better error messages for type mismatches
- [ ] String pool garbage collection
- [ ] String interning to avoid duplicates

## 🚀 Future Language Features
- [ ] Function declarations and calls (CALL, RET instructions)
- [ ] Variable scoping (stack frames, local variables)
- [ ] Control flow statements (if/else, loops with JUMP instructions)
- [ ] Arrays and data structures
- [ ] Standard library functions (print, input)
- [ ] Error handling and exceptions

## 📝 Current Architecture
**Complete Interpreter Pipeline**:
1. **Lexer** → Tokens ✅
2. **Parser** → AST ✅
3. **Semantic Analysis** → Type checking & validation ✅
4. **IR Generation** → Stack-based bytecode ✅
5. **Virtual Machine** → Execution ✅

**IR Instruction Set**: Stack-based with operations for constants, variables, arithmetic, comparison, logical operations, and string literals

**String Implementation**: Hybrid approach with IR storage and VM string pool for efficient execution

**String Concatenation**: Full implementation with proper type checking - string + string operations generate IR_CONCAT instructions while integer + integer operations generate IR_ADD instructions

## 📋 Development Notes
- See `improvements/string_literal_implementation.md` for detailed string implementation documentation
- Current string pool capacity: 50 strings (expandable)
- Memory management: All strings properly freed in IR and VM cleanup
- Test coverage: Basic string declaration works (`string x = "hello world"`)

## 🔧 Build Commands
- **Build**: `cmake -B build && cmake --build build`
- **Test**: `./build/Debug/spade.exe test_scripts/variable_declaration/string.sp`
