# String Literal Implementation

## Overview

This document describes the implementation of string literal support in the Spade compiler, including the approach taken, current state, and areas for improvement.

## Implementation Approach

### Design Decision: String Pool with IR Storage

We chose to implement string literals using a hybrid approach:

1. **IR Level**: String literals are stored directly in IR instructions using the `string_lit` field
2. **VM Level**: During execution, strings are copied to a VM string pool for efficient access
3. **Stack**: String indices (not strings themselves) are pushed onto the integer stack

### Architecture

```
Source Code: string x = "hello world"
     ↓
Lexer: TOKEN_STRING_LITERAL("hello world")
     ↓
Parser: AST_STRING_LITERAL with value "hello world"
     ↓
IR Generator: IR_PUSH_STRING_LIT with string_lit = "hello world"
     ↓
VM Execution: Copy to string_pool[0], push index 0 onto stack
```

## Files Modified

### Core Components

1. **spade.ir.h** - Added `string_lit` field to `IRInstruction` union
2. **spade.ir.c** - Added `emit_instruction_string_lit()` function
3. **spade.vm.h** - Added string pool fields to `VirtualMachine` struct
4. **spade.vm.c** - Added string pool management functions

### Key Functions Added

- `emit_instruction_string_lit()` - Emits string literal IR instructions
- `store_string()` - Adds strings to VM pool and pushes index to stack
- `load_string()` - Retrieves string from pool by index
- `peek_string_pool()` - Debug function to display string pool contents

## Current State

### Working Features

- ✅ String literal parsing and AST generation
- ✅ IR instruction generation for string literals
- ✅ VM string pool management
- ✅ Memory cleanup for IR strings and VM pool
- ✅ Debug output for string operations
- ✅ **String concatenation with proper type checking**
- ✅ **IR_CONCAT instruction for string operations**
- ✅ **Mixed type operations (string + string vs int + int)**

### Test Cases

The implementation passes multiple test cases:

**Basic String Literals:**
```spade
string x = "hello world";
```

**String Concatenation:**
```spade
string greeting = "Hello";
string name = "World";
string result = greeting + " " + name;  // → "Hello World"
```

**Integer Addition (still works):**
```spade
int x = 10;
int y = x + 2;  // → 12
```

## Areas for Improvement

### 1. String Operations

**Completed**: String concatenation (`str1 + str2`) ✅

**Remaining Gaps**:
- String comparison (`str1 == str2`, `str1 != str2`)
- String length function
- Substring operations

**Implementation Notes**:
- ✅ IR_CONCAT opcode implemented for concatenation
- Would need additional opcodes (IR_STR_CMP, IR_STR_LEN, etc.)
- VM would need additional string manipulation functions
- ✅ Memory management for temporary strings implemented

### 2. Type System Integration

**Current Gap**: String variables store integer indices instead of proper type information

**Suggested Improvements**:
- Extend symbol table to track string vs integer types
- Add type checking for string operations
- Separate string and integer stacks in VM

### 3. Memory Optimization

**Current Gap**: Strings are duplicated (IR → VM pool)

**Suggested Improvements**:
- Implement string interning to avoid duplicates
- Add string pool compaction during garbage collection
- Consider reference counting for string lifecycle

### 4. String Pool Management

**Current Gap**: String pool grows indefinitely

**Suggested Improvements**:
- Implement string pool garbage collection
- Add string pool size limits with overflow handling
- Pool statistics and monitoring

### 5. Error Handling

**Current Gap**: Limited error checking for string operations

**Suggested Improvements**:
- Better error messages for string index out of bounds
- Validation of string pool integrity
- Memory allocation failure handling

## Technical Debt

### Memory Management
- String literals in IR instructions are properly freed
- VM string pool is cleaned up in `free_VM()`
- No memory leaks detected in current implementation

### Performance Considerations
- String operations are O(n) for most cases
- String pool lookup is O(1) by index
- Memory usage scales with number of unique strings

## Future Enhancements

### Short Term (Next Sprint)
1. ✅ Implement basic string concatenation - COMPLETED
2. Add string comparison operators
3. Improve error messages for string operations

### Medium Term
1. Add string length and substring operations
2. Implement proper string type checking
3. Add string escape sequence support

### Long Term
1. String interpolation support
2. Regular expression integration
3. Unicode string support

## Contributing

When working on string-related improvements:

1. **Test Coverage**: Add test cases in `test_scripts/string/`
2. **Memory Safety**: Always use `strdup()` for string copying
3. **Error Handling**: Check return values from string operations
4. **Documentation**: Update this file with new features

## Related Files

- `test_scripts/variable_declaration/string.sp` - Basic string test
- `spade.lexer.c` - String literal tokenization  
- `spade.parser.c` - String literal AST generation
- `spade.semantic.c` - String type validation (future)

## Questions for Future Discussion

1. Should we implement copy-on-write for string operations?
2. How should we handle string encoding (UTF-8, ASCII)?
3. Should string pool be global or per-VM instance?
4. What's the appropriate string pool size limit?
