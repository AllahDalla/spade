#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "spade.vm.h"

/**
 * Safe integer power function with overflow detection
 * @param base The base number
 * @param exponent The exponent (must be >= 0)
 * @param result Pointer to store the result
 * @return VM_SUCCESS if calculation is safe, VM_INVALID_INSTRUCTION if overflow
 */
VMResult safe_int_power(int base, int exponent, int *result) {
    // Handle edge cases first
    if (exponent < 0) {
        printf("Error: Negative exponents not supported\n");
        return VM_INVALID_INSTRUCTION;
    }
    
    if (exponent == 0) {
        *result = 1;  // Any number to power 0 is 1
        return VM_SUCCESS;
    }
    
    if (base == 0) {
        *result = 0;  // 0 to any positive power is 0
        return VM_SUCCESS;
    }
    
    if (base == 1) {
        *result = 1;  // 1 to any power is 1
        return VM_SUCCESS;
    }
    
    if (base == -1) {
        *result = (exponent % 2 == 0) ? 1 : -1;  // (-1)^even = 1, (-1)^odd = -1
        return VM_SUCCESS;
    }
    
    // Set reasonable limits to prevent overflow
    if (exponent > 31) {
        printf("Error: Exponent %d too large (max 31)\n", exponent);
        return VM_INVALID_INSTRUCTION;
    }
    
    // Check for potential overflow using simple heuristics
    if (abs(base) > 2 && exponent > 15) {
        printf("Error: Power operation would overflow (base=%d, exp=%d)\n", base, exponent);
        return VM_INVALID_INSTRUCTION;
    }
    
    // Perform the calculation with overflow checking
    *result = 1;
    int current_base = base;
    
    for (int i = 0; i < exponent; i++) {
        // Check if multiplication would overflow
        if (*result > INT_MAX / abs(current_base)) {
            printf("Error: Power operation overflow detected\n");
            return VM_INVALID_INSTRUCTION;
        }
        *result *= current_base;
    }
    
    return VM_SUCCESS;
}

/**
 * Creates and initializes a new virtual machine
 * @return VirtualMachine struct with allocated stack and variables
 */
VirtualMachine createVirtualMachine(){
    VirtualMachine vm;
    vm.stack = malloc(sizeof(int) * 100);
    if (!vm.stack) {
        printf("Error: Failed to allocate stack memory\n");
        vm.machine_state = ERROR;
        return vm;
    }
    vm.stack_count = -1;
    vm.stack_capacity = 100;

    vm.variables = malloc(sizeof(Variable) * 10);
    if (!vm.variables) {
        printf("Error: Failed to allocate variables memory\n");
        free(vm.stack);
        vm.machine_state = ERROR;
        return vm;
    }

    vm.variable_count = -1;
    vm.variable_capacity = 10;
    
    vm.string_pool = malloc(sizeof(char *) * 50);
    if (!vm.string_pool) {
        printf("Error: Failed to allocate string pool memory\n");
        free(vm.stack);
        free(vm.variables);
        vm.machine_state = ERROR;
        return vm;
    }

    vm.string_pool_count = 0;
    vm.string_pool_capacity = 50;

    vm.program_counter = -1;
    vm.machine_state = RUNNING;
    return vm;
}


/**
 * Prints the current state of the virtual machine
 * @param vm Pointer to the virtual machine
 */
void print_VM_state(VirtualMachine *vm){
    printf("Virtual Machine State:\n");
    printf("Stack Capacity: %d\n", vm->stack_capacity);
    printf("Stack Count: %d\n", vm->stack_count);
    printf("Stack Contents: \n");
    peek_stack(vm);
    printf("Variable Capacity: %d\n", vm->variable_capacity);
    printf("Variable Count: %d\n", vm->variable_count);
    printf("Variable Contents: \n");
    peek_variables(vm);
    printf("String Pool Capacity: %d\n", vm->string_pool_capacity);
    printf("String Pool Count: %d\n", vm->string_pool_count);
    printf("String Pool Contents: \n");
    peek_string_pool(vm);
}

/**
 * Pushes a value onto the VM stack
 * @param vm Pointer to the virtual machine
 * @param value Integer value to push
 * @return VM_SUCCESS or VM_STACK_OVERFLOW
 */
VMResult push_stack(VirtualMachine *vm, int value){
    if(vm->stack_count == vm->stack_capacity - 1){
        printf("Stack Overflow\n");
        return VM_STACK_OVERFLOW;
    }

    vm->stack[++vm->stack_count] = value;
    return VM_SUCCESS;
}

/**
 * Pops a value from the VM stack
 * @param vm Pointer to the virtual machine
 * @param value Pointer to store the popped value
 * @return VM_SUCCESS or VM_STACK_UNDERFLOW
 */
VMResult pop_stack(VirtualMachine *vm, int *value){
    if(vm->stack_count == -1){
        printf("Stack Underflow\n");
        return VM_STACK_UNDERFLOW;
    }

    *value = vm->stack[vm->stack_count--];
    return VM_SUCCESS;
}

/**
 * Prints all values currently on the stack
 * @param vm Pointer to the virtual machine
 */
void peek_stack(VirtualMachine *vm){
    for(int i = 0; i <= vm->stack_count; i++){
        printf("        %d. %d \n", i + 1, vm->stack[i]);
    }
}

/**
 * Stores a variable in the VM's variable table
 * @param vm Pointer to the virtual machine
 * @param name Variable name (string will be copied)
 * @param value Integer value to store
 * @return VM_SUCCESS or VM_OUT_OF_MEMORY
 */
VMResult store_variable(VirtualMachine *vm, char *name, int value){
    Variable variable;
    variable.name = strdup(name);
    if (!variable.name) {
        return VM_OUT_OF_MEMORY;
    }
    variable.value = value;

    if(vm->variable_count >= vm->variable_capacity - 1){
        vm->variable_capacity *= 2;
        Variable *new_vars = realloc(vm->variables, sizeof(Variable) * vm->variable_capacity);
        if (!new_vars) {
            free(variable.name);
            return VM_OUT_OF_MEMORY;
        }
        vm->variables = new_vars;
    }

    vm->variables[++vm->variable_count] = variable;
    return VM_SUCCESS;
}

/**
 * Loads a variable value and pushes it onto the stack
 * @param vm Pointer to the virtual machine
 * @param name Variable name to look up
 * @return VM_SUCCESS or VM_VARIABLE_NOT_FOUND
 */
VMResult load_variable(VirtualMachine *vm, char *name){
    for(int i = 0; i <= vm->variable_count; i++){
        if(strcmp(vm->variables[i].name, name) == 0){
            push_stack(vm, vm->variables[i].value);
            return VM_SUCCESS;
        }
    }

    return VM_VARIABLE_NOT_FOUND;
}

/**
 * Frees all memory allocated by the virtual machine
 * @param vm Pointer to the virtual machine to free
 */
void free_VM(VirtualMachine *vm){
    if (vm->stack) {
        free(vm->stack);
        vm->stack = NULL;
    }

    if(vm->string_pool){
        for(int i = 0; i < vm->string_pool_count; i++){
            if(vm->string_pool[i]){
                free(vm->string_pool[i]);
            }
        }
        free(vm->string_pool);
        vm->string_pool = NULL;
    }
    
    if (vm->variables) {
        // Free variable names
        for (int i = 0; i <= vm->variable_count; i++) {
            if (vm->variables[i].name) {
                free(vm->variables[i].name);
            }
        }
        free(vm->variables);
        vm->variables = NULL;
    }
    
    vm->stack_count = -1;
    vm->variable_count = -1;
    vm->program_counter = -1;
    vm->machine_state = HALTED;
}

void peek_variables(VirtualMachine *vm){
    for(int i = 0; i <= vm->variable_count; i++){
        printf("        %d. %s = %d\n", i + 1, vm->variables[i].name, vm->variables[i].value);
    }
}


VMResult store_string(VirtualMachine *vm, char *string){
    if(vm->string_pool_count >= vm->string_pool_capacity - 1){
        vm->string_pool_capacity *= 2;
        char **new_pool = realloc(vm->string_pool, sizeof(char *) * vm->string_pool_capacity);
        if (!new_pool) {
            return VM_OUT_OF_MEMORY;
        }
        vm->string_pool = new_pool;
    }

    vm->string_pool[vm->string_pool_count] = strdup(string);
    push_stack(vm, vm->string_pool_count);
    vm->string_pool_count++;
    return VM_SUCCESS;
}  

VMResult load_string(VirtualMachine *vm, int index, char **string){
    if(index >= vm->string_pool_count || index < 0){
        printf("Error: String stack index out of bounds\n");
        return VM_INDEX_OUT_OF_BOUNDS;
    }

    *string = vm->string_pool[index];
    return VM_SUCCESS;
}

void peek_string_pool(VirtualMachine *vm){
    for(int i = 0; i < vm->string_pool_count; i++){
        printf("        %d. %s\n", i + 1, vm->string_pool[i]);
    }
}


/**
 * Executes IR code on the virtual machine
 * @param vm Pointer to the virtual machine
 * @param ir_code Pointer to the IR code to execute
 * @return VM_SUCCESS or appropriate error code
 */
VMResult execute_ir_code(VirtualMachine *vm, IRCode *ir_code){
    vm->program_counter = 0;
    vm->machine_state = RUNNING;
    
    while (vm->machine_state == RUNNING && vm->program_counter < ir_code->count) {
        IRInstruction *instr = &ir_code->instructions[vm->program_counter];
        
        switch (instr->opcode) {
            case IR_PUSH_CONST:
                // TODO: Implement push constant
                if(push_stack(vm, instr->operand.int_value) != VM_SUCCESS){
                    printf("Error: Failed to push constant %d unto stack\n", instr->operand.int_value);
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }

                break;
                
            case IR_PUSH_STRING_LIT:
                // Push string index onto stack
                if(store_string(vm, instr->operand.string_lit) != VM_SUCCESS){
                    printf("Error: Failed to store string %s in Virtual Machine\n", instr->operand.string_lit);
                    vm->machine_state = ERROR;
                    return VM_OUT_OF_MEMORY;
                }
                break;
                
            case IR_PUSH_VAR:
                // TODO: Implement push variable
                if(load_variable(vm, instr->operand.var_name) != VM_SUCCESS){
                    printf("Error: Failed to load variable %s\n", instr->operand.var_name);
                    vm->machine_state = ERROR;
                    return VM_VARIABLE_NOT_FOUND;
                }
                break;
                
            case IR_STORE_VAR: {
                // TODO: Implement store variable
                int value;
                if(pop_stack(vm, &value) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(store_variable(vm, instr->operand.var_name, value) != VM_SUCCESS){
                    printf("Error: Failed to store variable %s MACHINE OUT OF MEMORY\n", instr->operand.var_name);
                    vm->machine_state = ERROR;
                    return VM_OUT_OF_MEMORY;
                }
                break;
            }
                
            case IR_ADD: {
                // TODO: Implement addition
                int left, right;

                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                
                if(push_stack(vm, left + right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_SUB: {
                // TODO: Implement subtraction
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }

                if(push_stack(vm, left - right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_MUL: {
                // TODO: Implement multiplication
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }

                if(push_stack(vm, left * right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_DIV: {
                // TODO: Implement division
                int left, right;

                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }

                if(right == 0){
                    printf("Error: Division by zero\n");
                    vm->machine_state = ERROR;
                    return VM_INVALID_INSTRUCTION;
                }

                if(push_stack(vm, left / right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }

                break;
            }
                
            case IR_MOD: {
                // TODO: Implement modulo
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(right == 0){
                    printf("Error: Modulo by zero\n");
                    vm->machine_state = ERROR;
                    return VM_INVALID_INSTRUCTION;
                }
                if(push_stack(vm, left % right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_POW: {
                // Implement safe power with overflow detection
                int base, exponent;
                if(pop_stack(vm, &exponent) != VM_SUCCESS || pop_stack(vm, &base) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                
                int power_result;
                VMResult safe_result = safe_int_power(base, exponent, &power_result);
                if (safe_result != VM_SUCCESS) {
                    printf("Error: Power operation failed (base=%d, exp=%d)\n", base, exponent);
                    vm->machine_state = ERROR;
                    return safe_result;
                }
                
                if(push_stack(vm, power_result) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_EQ: {
                // TODO: Implement equal comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left == right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_NE: {
                // TODO: Implement not equal comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left != right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_LT: {
                // TODO: Implement less than comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left < right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_GT: {
                // TODO: Implement greater than comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left > right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_LE: {
                // TODO: Implement less equal comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left <= right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_GE: {
                // TODO: Implement greater equal comparison
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, left >= right) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_AND: {
                // TODO: Implement logical and
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, (left && right) ? 1 : 0) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_OR: {
                // TODO: Implement logical or
                int left, right;
                if(pop_stack(vm, &right) != VM_SUCCESS || pop_stack(vm, &left) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, (left || right) ? 1 : 0) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_NOT: {
                // TODO: Implement logical not
                int value;
                if(pop_stack(vm, &value) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, !value ? 1 : 0) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_NEG: {
                // TODO: Implement negation
                int value;
                if(pop_stack(vm, &value) != VM_SUCCESS){
                    printf("Error: Stack Underflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_UNDERFLOW;
                }
                if(push_stack(vm, -value) != VM_SUCCESS){
                    printf("Error: Stack Overflow\n");
                    vm->machine_state = ERROR;
                    return VM_STACK_OVERFLOW;
                }
                break;
            }
                
            case IR_HALT:
                vm->machine_state = HALTED;
                break;
                
            default:
                vm->machine_state = ERROR;
                return VM_INVALID_INSTRUCTION;
        }
        
        vm->program_counter++;
    }
    
    return VM_SUCCESS;
}