#ifndef SPADE_SYM_H
#define SPADE_SYM_H

#define MAX_SYMBOLS 1024

typedef struct {
    char *name;
    enum TokenType type;
}Symbol;

typedef struct {
    Symbol *symbols[MAX_SYMBOLS];
    int count;
}SymbolTable;

extern SymbolTable global_symbol_table;

int add_symbol(SymbolTable *table, const char *name, enum TokenType type);
Symbol *lookup_symbol_table(SymbolTable *table, const char *name);
void free_symbol_table(SymbolTable *table);
void print_symbol_table(SymbolTable *table);

#endif