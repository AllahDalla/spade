#ifndef SPADE_VM_H
#define SPADE_VM_H

#include "spade.ir.h"

typedef enum {
    RUNNING,
    HALTED,
    ERROR
}ExecutionState;

typedef enum {
    VM_SUCCESS,
    VM_STACK_OVERFLOW,
    VM_STACK_UNDERFLOW,
    VM_INDEX_OUT_OF_BOUNDS,
    VM_VARIABLE_NOT_FOUND,
    VM_OUT_OF_MEMORY,
    VM_DIVISION_BY_ZERO,
    VM_INVALID_INSTRUCTION,
} VMResult;

typedef struct {
    char *name;
    int value;
}Variable;

typedef struct {

    int *stack;
    int stack_count;
    int stack_capacity;

    char **string_pool;
    int string_pool_count;
    int string_pool_capacity;

    Variable *variables;
    int variable_count;
    int variable_capacity;

    int program_counter;
    ExecutionState machine_state;

}VirtualMachine;

VirtualMachine createVirtualMachine();
void print_VM_state(VirtualMachine *vm);
void free_VM(VirtualMachine *vm);

VMResult push_stack(VirtualMachine *vm, int value);
VMResult pop_stack(VirtualMachine *vm, int *value);
void peek_stack(VirtualMachine *vm);

VMResult store_variable(VirtualMachine *vm, char *name, int value);
VMResult load_variable(VirtualMachine *vm, char *name);
void peek_variables(VirtualMachine *vm);

VMResult store_string(VirtualMachine *vm, char *string);
VMResult load_string(VirtualMachine *vm, int index, char **string);
void peek_string_pool(VirtualMachine *vm);

// internal functions
Variable *lookup_variable(VirtualMachine *vm, char *name);
int update_variable(VirtualMachine *vm, char *name, int value);


VMResult execute_ir_code(VirtualMachine *vm, IRCode *ir_code);

#endif