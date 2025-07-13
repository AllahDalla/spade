#ifndef SPADE_IR_H
#define SPADE_IR_H

#include "spade.lexer.h"
#include "spade.parser.h"

typedef enum {
    IR_PUSH_CONST,      // Push constant value
    IR_PUSH_VAR,        // Push variable value
    IR_STORE_VAR,       // Store top of stack to variable
    IR_ADD,             // Pop two, add, push result
    IR_SUB,             // Pop two, subtract, push result
    IR_MUL,             // Pop two, multiply, push result
    IR_DIV,             // Pop two, divide, push result
    IR_MOD,             // Pop two, modulo, push result
    IR_POW,             // Pop two, power, push result
    IR_EQ,              // Pop two, compare equal, push result
    IR_NE,              // Pop two, compare not equal, push result
    IR_LT,              // Pop two, compare less than, push result
    IR_GT,              // Pop two, compare greater than, push result
    IR_LE,              // Pop two, compare less equal, push result
    IR_GE,              // Pop two, compare greater equal, push result
    IR_AND,             // Pop two, logical and, push result
    IR_OR,              // Pop two, logical or, push result
    IR_NOT,             // Pop one, logical not, push result
    IR_NEG,             // Pop one, negate, push result
    IR_HALT             // End of program
} IROpcode;

typedef struct {
    IROpcode opcode;
    union {
        int int_value;      // For constants
        char *var_name;     // For variable operations
    } operand;
} IRInstruction;

typedef struct {
    IRInstruction *instructions;
    int count;
    int capacity;
} IRCode;

// Function declarations
IRCode *create_ir_code(void);
void emit_instruction(IRCode *code, IROpcode opcode);
void emit_instruction_int(IRCode *code, IROpcode opcode, int value);
void emit_instruction_var(IRCode *code, IROpcode opcode, const char *var_name);
void generate_ir(ASTNode *ast, IRCode *code);
void print_ir_code(IRCode *code);
void free_ir_code(IRCode *code);

#endif