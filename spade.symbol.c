#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.lexer.h"
#include "spade.symbol.h"

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


Symbol *lookup_symbol_table(SymbolTable *table, const char *name){
    // search for symbol in table
    for(int i = 0; i < table->count; i++){
        if(strcmp(table->symbols[i]->name, name) == 0){
            return table->symbols[i]; // return symbol if found
        }
    }
    return NULL; // return NULL if symbol not found
}


void print_symbol_table(SymbolTable *table){
    printf("Symbol Table:\n");
    for(int i = 0; i < table->count; i++){
        printf("%s: %s\n", table->symbols[i]->name, get_token_name(table->symbols[i]->type));
    }
}


void free_symbol_table(SymbolTable *table){
    for(int i = 0; i < table->count; i++){
        free(table->symbols[i]->name); // free the name of the symbol
        free(table->symbols[i]); // free the symbol itself
    }

    table->count = 0; // reset the count to 0
}
