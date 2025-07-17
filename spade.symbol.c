#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.lexer.h"
#include "spade.symbol.h"

/**
 * Adds a new symbol to the symbol table.
 *
 * @param table The symbol table to add the symbol to
 * @param name The name of the symbol to be added
 * @param type The token type of the symbol
 * @return 1 if symbol was successfully added, 0 if symbol already exists or table is full
 */
int add_symbol(SymbolTable *table, const char *name, enum TokenType type) {
    // check if symbol already exists
    for(int i = 0; i < table->count; i++){
        if(strcmp(table->symbols[i]->name, name) == 0){
            return 0; // return false if symbol already exists
        }
    }

    // check if table is full
    if(table->count >= MAX_SYMBOLS) return 0; // return false if table is full
    // add symbol to table
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->params = NULL;
    new_symbol->param_count = 0;
    new_symbol->param_capacity = 0;
    new_symbol->local_scope = NULL;
    table->symbols[table->count++] = new_symbol;
    return 1; // return true if symbol was added successfully
}

/**
 * Adds a new function symbol to the symbol table with parameters and local scope.
 *
 * Creates a function symbol with its own parameter list and local symbol table.
 * The function's parameters are automatically added to its local scope for
 * proper scoping during semantic analysis.
 *
 * @param table The symbol table to add the function to
 * @param name The name of the function to be added
 * @param type The return type of the function (TOKEN_INT, TOKEN_STRING, etc.)
 * @param params Array of parameters for the function
 * @param param_count Number of parameters in the params array
 * @return 1 if function was successfully added, 0 if function already exists or table is full
 */
int add_symbol_function(SymbolTable *table, const char *name, enum TokenType type, Param *params, int param_count) {
    // check if symbol already exists in local scope
    for(int i = 0; i < table->count; i++){
        if(strcmp(table->symbols[i]->name, name) == 0){
            return 0; // return false if symbol already exists
        }
    }

    // check if table is full
    if(table->count >= MAX_SYMBOLS) return 0; // return false if table is full

    // create local symbol table and add its parent
    SymbolTable *local_scope = (SymbolTable *)malloc(sizeof(SymbolTable));
    local_scope->parent = table;
    local_scope->count = 0;
    
    // Initialize all symbol pointers to NULL
    for(int i = 0; i < MAX_SYMBOLS; i++) {
        local_scope->symbols[i] = NULL;
    }

    // add symbol to table
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->param_count = param_count;
    new_symbol->param_capacity = param_count > 0 ? param_count : 10;
    new_symbol->local_scope = local_scope;

    // Allocate memory for parameters array and copy parameter data
    if(param_count > 0) {
        new_symbol->params = (Param *)malloc(sizeof(Param) * param_count);
        for(int i = 0; i < param_count; i++){
            new_symbol->params[i].name = strdup(params[i].name);  // Deep copy parameter name
            new_symbol->params[i].type = params[i].type;
        }
    } else {
        new_symbol->params = NULL;  // No parameters for this function
    }

    // Add function parameters to the function's local scope
    for (int i = 0; i < param_count; i++){
        add_symbol(local_scope, params[i].name, params[i].type);
    }
    table->symbols[table->count++] = new_symbol;
    return 1;
    
}


/**
 * Searches for a symbol in the symbol table by its name.
 *
 * @param table The symbol table to search in
 * @param name The name of the symbol to look up
 * @return A pointer to the Symbol if found, NULL otherwise
 */
Symbol *lookup_symbol_table(SymbolTable *table, const char *name){
    // search for symbol in table
    for(int i = 0; i < table->count; i++){
        if(strcmp(table->symbols[i]->name, name) == 0){
            return table->symbols[i]; // return symbol if found
        }
    }

    if(table->parent != NULL){
        return lookup_symbol_table(table->parent, name); // recursively search parent scope
    }

    return NULL; // return NULL if symbol not found
}

/**
 * Searches for a function symbol in the symbol table by name and parameter signature.
 *
 * Performs function overload resolution by matching both the function name and
 * parameter types. Recursively searches parent scopes if the function is not
 * found in the current scope.
 *
 * @param table The symbol table to search in
 * @param name The name of the function to look up
 * @param params Array of parameters to match against function signature
 * @param param_count Number of parameters in the params array
 * @return A pointer to the Symbol if found with matching signature, NULL otherwise
 */
Symbol *lookup_symbol_table_function(SymbolTable *table, const char *name, Param *params, int param_count){
    // search for symbol in table
    int flag = 1;  // Flag to track parameter type matching
    for(int i = 0; i < table->count; i++){
        if(strcmp(table->symbols[i]->name, name) == 0){  // Found matching function name
            if(table->symbols[i]->param_count == param_count){  // Check parameter count match
                // Check each parameter type for exact match
                for(int j = 0; j < param_count; j++){
                    if(table->symbols[i]->params[j].type != params[j].type){
                        flag = 0;  // Parameter type mismatch
                        break;
                    }
                }
                if(flag){  // All parameters matched
                    return table->symbols[i]; // return symbol if found
                }
            }
        }
        flag = 1;  // Reset flag for next symbol
    }

    if(table->parent != NULL){
        return lookup_symbol_table_function(table->parent, name, params, param_count); // recursively search parent scope
    }
    return NULL; // return NULL if symbol not found
}


/**
 * Prints the contents of the symbol table to the console.
 *
 * Iterates through the symbol table and displays each symbol's name 
 * and its corresponding token type name.
 *
 * @param table The symbol table to be printed
 */
void print_symbol_table(SymbolTable *table){
    printf("Symbol Table:\n");
    for(int i = 0; i < table->count; i++){
        printf("%s: %s\n", table->symbols[i]->name, get_token_name(table->symbols[i]->type));
    }
}


/**
 * Frees the memory allocated for a symbol table.
 *
 * Iterates through the symbols in the table, freeing the memory 
 * for each symbol's name, parameters, local scope, and the symbol itself.
 * Resets the table's symbol count to 0 after freeing.
 *
 * @param table The symbol table to be freed
 */
void free_symbol_table(SymbolTable *table){
    for(int i = 0; i < table->count; i++){
        Symbol *symbol = table->symbols[i];
        
        // Free symbol name
        if(symbol->name != NULL) {
            free(symbol->name);
        }
        
        // Free function parameters
        if(symbol->params != NULL) {
            for(int j = 0; j < symbol->param_count; j++) {
                if(symbol->params[j].name != NULL) {
                    free(symbol->params[j].name);
                }
            }
            free(symbol->params);
        }
        
        // Free local scope (recursively)
        if(symbol->local_scope != NULL) {
            free_symbol_table(symbol->local_scope);
            free(symbol->local_scope);
        }
        
        // Free the symbol itself
        free(symbol);
        table->symbols[i] = NULL;
    }

    table->count = 0; // reset the count to 0
}
