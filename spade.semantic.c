#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.parser.h"
#include "spade.symbol.h"


/**
 * Recursively analyzes an Abstract Syntax Tree (AST) for semantic correctness.
 * 
 * This function performs semantic analysis on different types of AST nodes, including:
 * - Variable declarations (adding symbols to the symbol table)
 * - Identifier resolution and validation
 * - Binary operation traversal
 * 
 * @param tree The root or current node of the Abstract Syntax Tree to analyze
 * @param symbol_table The symbol table used for tracking declared variables and their types
 */
void analyze_AST(ASTNode *tree, SymbolTable *symbol_table){
    if(tree == NULL){
        return;
    }

    switch(tree->type){
        case AST_VARIABLE_DECLARATION:{
             // add the variable being declared to the symbol table
            if(!add_symbol(symbol_table, tree->data.var_declaration.name , tree->data.var_declaration.var_type)){
                if(symbol_table->count >= MAX_SYMBOLS){
                    // symbol table is full and free the symbol table and free the AST node
                    printf("Error: Symbol table is full\n");
                    return;
                }
                printf("Error: Symbol already exists\n");
                return;
            }

            if(tree->data.var_declaration.value != NULL){
                analyze_AST(tree->data.var_declaration.value, symbol_table);
            }
            break;

        }

        case AST_IDENTIFIER: {
            if(tree->data.identifier.name == NULL){
                printf("Error: Identifier is NULL\n");
                return;
            }

            if(lookup_symbol_table(symbol_table, tree->data.identifier.name) == NULL){
                printf("Error: Identifier does not exists\n");
                return;
            }
            break;
        }

        case AST_BINARY_OPERATION: {
            analyze_AST(tree->data.bin_op.left, symbol_table);
            analyze_AST(tree->data.bin_op.right, symbol_table);
            break;
        }
    }
}