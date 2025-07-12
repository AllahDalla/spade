# Spade Compiler Development TODOs

## ✅ Completed
- [x] Lexer implementation with token recognition
- [x] Parser with AST generation for variable declarations  
- [x] Symbol table for variable tracking
- [x] Semantic analysis with type checking
- [x] Error detection for type mismatches, undeclared variables, redeclarations
- [x] Test cases for semantic error validation

## 🔄 Next: Stack-Based IR Generation

### Phase 1: Core IR Infrastructure
- [ ] Design instruction set (PUSH_CONST, PUSH_VAR, ADD, SUB, MUL, etc.)
- [ ] Define instruction structure (opcode + optional operand)  
- [ ] Create IR instruction enum and data structures
- [ ] Implement virtual stack for runtime evaluation

### Phase 2: Code Generation
- [ ] Create `generate_IR(ASTNode* ast)` function
- [ ] Implement post-order AST traversal for expression IR
- [ ] Handle variable declarations (STORE_VAR instructions)
- [ ] Handle binary operations (arithmetic, comparison, logical)
- [ ] Connect IR generation after semantic analysis

### Phase 3: Testing & Integration
- [ ] Test IR generation with existing test cases
- [ ] Debug and validate instruction sequences
- [ ] Add IR output to compiler pipeline
- [ ] Create IR execution/interpretation capability

## 🚀 Future Features
- [ ] Function declarations and calls
- [ ] Variable scoping (block scope, function scope)
- [ ] Control flow (if/else, loops) 
- [ ] Final code generation (assembly/machine code)

## 📝 Implementation Strategy
**Stack-based IR approach**: Use virtual stack machine with RPN-style instruction sequences
- Expression `a + b * c` becomes: `PUSH a, PUSH b, PUSH c, MUL, ADD`
- Variable assignment `x = result` becomes: `STORE_VAR x`
- Natural mapping from expression trees to stack operations
