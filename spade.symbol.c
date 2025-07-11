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
    table->symbols[table->count++] = new_symbol;
    return 1; // return true if symbol was added successfully
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
 * for each symbol's name and the symbol itself. Resets the table's 
 * symbol count to 0 after freeing.
 *
 * @param table The symbol table to be freed
 */
void free_symbol_table(SymbolTable *table){
    for(int i = 0; i < table->count; i++){
        free(table->symbols[i]->name); // free the name of the symbol
        free(table->symbols[i]); // free the symbol itself
    }

    table->count = 0; // reset the count to 0
}
