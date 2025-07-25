#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.ir.h"
#include "spade.vm.h"
#include "spade.symbol.h"

/**
 * Helper function to determine if an AST node represents a string type
 * @param ast The AST node to check
 * @param symbol_table The symbol table to look up variable types
 * @return 1 if the node represents a string, 0 otherwise
 */
int is_string_type(ASTNode *ast, SymbolTable *symbol_table) {
    if (!ast) return 0;
    
    switch (ast->type) {
        case AST_STRING_LITERAL:
            return 1;
            
        case AST_IDENTIFIER: {
            // Look up the variable in the symbol table
            for (int i = 0; i < symbol_table->count; i++) {
                if (strcmp(symbol_table->symbols[i]->name, ast->data.identifier.name) == 0) {
                    return symbol_table->symbols[i]->type == TOKEN_STRING;
                }
            }
            return 0;  // Variable not found, assume not string
        }
        
        case AST_BINARY_OPERATION:
            // If it's a + operation between strings, the result is a string
            if (ast->data.bin_op.op == TOKEN_PLUS) {
                return is_string_type(ast->data.bin_op.left, symbol_table) || 
                       is_string_type(ast->data.bin_op.right, symbol_table);
            }
            return 0;
            
        default:
            return 0;
    }
}

/**
 * Creates and initializes a new IR code container.
 * 
 * Allocates memory for an IRCode structure and initializes it with
 * a default capacity for storing IR instructions.
 * 
 * @return A pointer to the newly created IRCode structure
 */
IRCode *create_ir_code(void) {
    IRCode *code = malloc(sizeof(IRCode));
    code->capacity = 100;
    code->count = 0;
    code->instructions = malloc(sizeof(IRInstruction) * code->capacity);
    return code;
}

/**
 * Emits a simple IR instruction with no operands.
 * 
 * Adds an instruction to the IR code sequence. Automatically resizes
 * the instruction array if capacity is exceeded.
 * 
 * @param code The IR code container to add the instruction to
 * @param opcode The instruction opcode (e.g., IR_ADD, IR_HALT)
 */
void emit_instruction(IRCode *code, IROpcode opcode) {
    if (code->count >= code->capacity) {
        code->capacity *= 2;
        code->instructions = realloc(code->instructions, 
                                   sizeof(IRInstruction) * code->capacity);
    }
    
    code->instructions[code->count].opcode = opcode;
    code->count++;
}

/**
 * Emits an IR instruction with an integer operand.
 * 
 * Adds an instruction with an integer value to the IR code sequence.
 * Used for constants and immediate values.
 * 
 * @param code The IR code container to add the instruction to
 * @param opcode The instruction opcode (e.g., IR_PUSH_CONST)
 * @param value The integer value to associate with the instruction
 */
void emit_instruction_int(IRCode *code, IROpcode opcode, int value) {
    if (code->count >= code->capacity) {
        code->capacity *= 2;
        code->instructions = realloc(code->instructions, 
                                   sizeof(IRInstruction) * code->capacity);
    }
    
    code->instructions[code->count].opcode = opcode;
    code->instructions[code->count].operand.int_value = value;
    code->count++;
}

/**
 * Emits an IR instruction with a variable name operand.
 * 
 * Adds an instruction with a variable name to the IR code sequence.
 * The variable name is duplicated and stored in the instruction.
 * 
 * @param code The IR code container to add the instruction to
 * @param opcode The instruction opcode (e.g., IR_PUSH_VAR, IR_STORE_VAR)
 * @param var_name The variable name to associate with the instruction
 */
void emit_instruction_var(IRCode *code, IROpcode opcode, const char *var_name) {
    if (code->count >= code->capacity) {
        code->capacity *= 2;
        code->instructions = realloc(code->instructions, 
                                   sizeof(IRInstruction) * code->capacity);
    }
    
    code->instructions[code->count].opcode = opcode;
    code->instructions[code->count].operand.var_name = strdup(var_name);
    code->count++;
}

/**
 * Emits an IR instruction with a string literal operand.
 * 
 * Adds an instruction with a string literal to the IR code sequence.
 * The string literal is duplicated and stored in the instruction for
 * later processing by the virtual machine.
 * 
 * @param code The IR code container to add the instruction to
 * @param opcode The instruction opcode (e.g., IR_PUSH_STRING_LIT)
 * @param string_lit The string literal to associate with the instruction
 */
void emit_instruction_string_lit(IRCode *code, IROpcode opcode, const char *string_lit) {
    if (code->count >= code->capacity) {
        code->capacity *= 2;
        code->instructions = realloc(code->instructions,
                                   sizeof(IRInstruction) * code->capacity);
    }

    code->instructions[code->count].opcode = opcode;
    code->instructions[code->count].operand.string_lit = strdup(string_lit);
    code->count++;
}


/**
 * Recursively generates IR code from an Abstract Syntax Tree.
 * 
 * Performs post-order traversal of the AST to generate stack-based
 * IR instructions. For expressions, operands are generated before
 * operators to maintain proper stack ordering.
 * 
 * @param ast The AST node to generate IR code for
 * @param code The IR code container to emit instructions to
 * @param vm The virtual machine for string pool management
 */
void generate_ir(ASTNode *ast, IRCode *code, SymbolTable *symbol_table) {
    if (!ast) return;
    
    switch (ast->type) {
        case AST_PROGRAM: {
            // Generate IR for all statements in the program
            for(int i = 0; i < ast->data.program.statement_count; i++){
                generate_ir(ast->data.program.statements[i], code, symbol_table);
            }
            break;
        }
        
        case AST_NUMBER:
            emit_instruction_int(code, IR_PUSH_CONST, ast->data.number.value);
            break;
            
        case AST_IDENTIFIER:
            emit_instruction_var(code, IR_PUSH_VAR, ast->data.identifier.name);
            break;
            
        case AST_BOOLEAN:
            emit_instruction_int(code, IR_PUSH_CONST, ast->data.boolean.value);
            break;
            
        case AST_BINARY_OPERATION:
            // Generate IR for left operand first
            generate_ir(ast->data.bin_op.left, code, symbol_table);
            // Generate IR for right operand second
            generate_ir(ast->data.bin_op.right, code, symbol_table);
            
            // Then emit the operation
            switch (ast->data.bin_op.op) {
                case TOKEN_PLUS:{
                    // Check if either operand is a string type
                    int left_is_string = is_string_type(ast->data.bin_op.left, symbol_table);
                    int right_is_string = is_string_type(ast->data.bin_op.right, symbol_table);
                    
                    // If either operand is a string, treat as string concatenation
                    if (left_is_string || right_is_string) {
                        emit_instruction(code, IR_CONCAT);
                    } else {
                        // Both operands are non-string types, use regular addition
                        emit_instruction(code, IR_ADD);
                    }
                    break;
                }     
                case TOKEN_MINUS:    emit_instruction(code, IR_SUB); break;
                case TOKEN_MULTIPLY: emit_instruction(code, IR_MUL); break;
                case TOKEN_DIVIDE:   emit_instruction(code, IR_DIV); break;
                case TOKEN_MODULO:   emit_instruction(code, IR_MOD); break;
                case TOKEN_POWER:    emit_instruction(code, IR_POW); break;
                case TOKEN_EQUALS:   emit_instruction(code, IR_EQ); break;
                case TOKEN_NOT_EQUALS: emit_instruction(code, IR_NE); break;
                case TOKEN_LESS_THAN: emit_instruction(code, IR_LT); break;
                case TOKEN_GREATER_THAN: emit_instruction(code, IR_GT); break;
                case TOKEN_LESS_THAN_EQUALS: emit_instruction(code, IR_LE); break;
                case TOKEN_GREATER_THAN_EQUALS: emit_instruction(code, IR_GE); break;
                case TOKEN_AND:      emit_instruction(code, IR_AND); break;
                case TOKEN_OR:       emit_instruction(code, IR_OR); break;
                default:
                    printf("Unknown binary operator in IR generation\n");
            }
            break;
            
        case AST_VARIABLE_DECLARATION:
            // Generate IR for the initializer expression if present
            if (ast->data.var_declaration.value) {
                generate_ir(ast->data.var_declaration.value, code, symbol_table);
                emit_instruction_var(code, IR_STORE_VAR, ast->data.var_declaration.name);
            }
            break;

        case AST_ASSIGNMENT: {
            // look up the variable in the symbol table
            Symbol *symbol = lookup_symbol_table(symbol_table, ast->data.variable_assignment.name);
            if(symbol){
                if(ast->data.variable_assignment.value){
                    generate_ir(ast->data.variable_assignment.value, code, symbol_table);
                    emit_instruction_var(code, IR_STORE_VAR, symbol->name);
                }
            }
            break;
        }
            
        case AST_UNARY_OPERATION:
            generate_ir(ast->data.unary_op.operand, code, symbol_table);
            switch (ast->data.unary_op.op) {
                case TOKEN_MINUS: emit_instruction(code, IR_NEG); break;
                case TOKEN_NOT:   emit_instruction(code, IR_NOT); break;
                default:
                    printf("Unknown unary operator in IR generation\n");
            }
            break;

        case AST_STRING_LITERAL: {
            // Add string to pool and emit index
            emit_instruction_string_lit(code, IR_PUSH_STRING_LIT, ast->data.string_lit.value);
            break;
        }
            
        default:
            printf("Unknown AST node type in IR generation\n");
    }
}

/**
 * Prints the IR code instructions to the console for debugging.
 * 
 * Displays each instruction with its index, opcode, and operands
 * in a human-readable format.
 * 
 * @param code The IR code container to print
 */
void print_ir_code(IRCode *code) {
    printf("\n=== IR CODE ===\n");
    for (int i = 0; i < code->count; i++) {
        IRInstruction *instr = &code->instructions[i];
        printf("%3d: ", i);
        
        switch (instr->opcode) {
            case IR_PUSH_CONST: printf("PUSH_CONST %d\n", instr->operand.int_value); break;
            case IR_PUSH_VAR:   printf("PUSH_VAR %s\n", instr->operand.var_name); break;
            case IR_PUSH_STRING_LIT: printf("PUSH_STRING_LIT \"%s\"\n", instr->operand.string_lit); break;
            case IR_STORE_VAR:  printf("STORE_VAR %s\n", instr->operand.var_name); break;
            case IR_CONCAT:     printf("CONCAT\n"); break;
            case IR_ADD:        printf("ADD\n"); break;
            case IR_SUB:        printf("SUB\n"); break;
            case IR_MUL:        printf("MUL\n"); break;
            case IR_DIV:        printf("DIV\n"); break;
            case IR_MOD:        printf("MOD\n"); break;
            case IR_POW:        printf("POW\n"); break;
            case IR_EQ:         printf("EQ\n"); break;
            case IR_NE:         printf("NE\n"); break;
            case IR_LT:         printf("LT\n"); break;
            case IR_GT:         printf("GT\n"); break;
            case IR_LE:         printf("LE\n"); break;
            case IR_GE:         printf("GE\n"); break;
            case IR_AND:        printf("AND\n"); break;
            case IR_OR:         printf("OR\n"); break;
            case IR_NOT:        printf("NOT\n"); break;
            case IR_NEG:        printf("NEG\n"); break;
            case IR_HALT:       printf("HALT\n"); break;
        }
    }
}

/**
 * Frees memory allocated for an IR code container and its instructions.
 * 
 * Releases memory for variable names stored in instructions, the instruction
 * array, and the IRCode structure itself.
 * 
 * @param code The IR code container to be freed
 */
void free_ir_code(IRCode *code) {
    if(!code) return;

    // Free allocated strings in instructions
    for(int i = 0; i < code->count; i++){
        IRInstruction *instr = &code->instructions[i];
        if(instr->opcode == IR_PUSH_VAR || instr->opcode == IR_STORE_VAR){
            if(instr->operand.var_name != NULL){
                free(instr->operand.var_name);
            }
        }
        else if(instr->opcode == IR_PUSH_STRING_LIT){
            if(instr->operand.string_lit != NULL){
                free(instr->operand.string_lit);
            }
        }
    }

    free(code->instructions);
    free(code);
}