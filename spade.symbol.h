#ifndef SPADE_SYM_H
#define SPADE_SYM_H

#define MAX_SYMBOLS 1024

// Forward declarations
typedef struct SymbolTable SymbolTable;

typedef struct {
    char *name;                   // Parameter name
    enum TokenType type;          // Parameter type (TOKEN_INT, TOKEN_STRING, etc.)
} Param;

typedef struct {
    char *name;                   // Symbol name (variable or function)
    enum TokenType type;          // Symbol type (TOKEN_INT, TOKEN_STRING, etc.)
    Param *params;                // Function parameters (NULL for variables)
    int param_count;              // Number of parameters (0 for variables)
    int param_capacity;           // Allocated capacity for parameters array
    SymbolTable *local_scope;     // Function's local symbol table (NULL for variables)
} Symbol;

typedef struct SymbolTable {
    Symbol *symbols[MAX_SYMBOLS]; // Array of symbol pointers
    int count;                    // Number of symbols in this table
    struct SymbolTable *parent;   // Parent scope (NULL for global scope)
} SymbolTable;

extern SymbolTable global_symbol_table;

// Symbol table manipulation functions
int add_symbol(SymbolTable *table, const char *name, enum TokenType type);                    // Add variable symbol
int add_symbol_function(SymbolTable *table, const char *name, enum TokenType type,          // Add function symbol with parameters
                       Param *params, int param_count);

// Symbol lookup functions
Symbol *lookup_symbol_table(SymbolTable *table, const char *name);                          // Find symbol by name
Symbol *lookup_symbol_table_function(SymbolTable *table, const char *name,                  // Find function by name and signature
                                    Param *params, int param_count);

// Symbol table utility functions
void free_symbol_table(SymbolTable *table);                                                 // Free all memory in symbol table
void print_symbol_table(SymbolTable *table); 

#endif