#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.parser.h"
#include "spade.symbol.h"

/**
 * Recursively determines the type of an expression by analyzing its AST structure.
 * 
 * @param expr The expression AST node to analyze
 * @param symbol_table The symbol table for variable lookups
 * @return The TokenType representing the expression's type, or -1 for type errors
 */
enum TokenType get_expression_type(ASTNode *expr, SymbolTable *symbol_table) {
    if (!expr) return -1;

    switch (expr->type) {
        case AST_NUMBER:
            return TOKEN_INT;
            
        case AST_BOOLEAN:
            return TOKEN_BOOL;
            
        case AST_STRING_LITERAL:
            return TOKEN_STRING;
            
        case AST_NULL:
            return TOKEN_NULL;
            
        case AST_IDENTIFIER: {
            Symbol *sym = lookup_symbol_table(symbol_table, expr->data.identifier.name);
            if (sym) return sym->type;
            printf("Error: Undeclared variable '%s'\n", expr->data.identifier.name);
            return -1;
        }
        
        case AST_BINARY_OPERATION: {
            enum TokenType left_type = get_expression_type(expr->data.bin_op.left, symbol_table);
            enum TokenType right_type = get_expression_type(expr->data.bin_op.right, symbol_table);
            
            if (left_type == -1 || right_type == -1) return -1;
            
            // Arithmetic operators: +, -, *, /, %
            if (expr->data.bin_op.op == TOKEN_PLUS || expr->data.bin_op.op == TOKEN_MINUS ||
                expr->data.bin_op.op == TOKEN_MULTIPLY || expr->data.bin_op.op == TOKEN_DIVIDE ||
                expr->data.bin_op.op == TOKEN_MODULO || expr->data.bin_op.op == TOKEN_POWER) {
                if (left_type == TOKEN_INT && right_type == TOKEN_INT) {
                    return TOKEN_INT;
                }
                printf("Error: Arithmetic operations require int operands\n");
                return -1;
            }
            
            // Comparison operators: <, >, <=, >=, ==, !=
            if (expr->data.bin_op.op == TOKEN_LESS_THAN || expr->data.bin_op.op == TOKEN_GREATER_THAN ||
                expr->data.bin_op.op == TOKEN_LESS_THAN_EQUALS || expr->data.bin_op.op == TOKEN_GREATER_THAN_EQUALS ||
                expr->data.bin_op.op == TOKEN_EQUALS || expr->data.bin_op.op == TOKEN_NOT_EQUALS) {
                if (left_type == right_type) {
                    return TOKEN_BOOL;
                }
                printf("Error: Comparison requires operands of same type\n");
                return -1;
            }
            
            // Logical operators: &&, ||
            if (expr->data.bin_op.op == TOKEN_AND || expr->data.bin_op.op == TOKEN_OR) {
                if (left_type == TOKEN_BOOL && right_type == TOKEN_BOOL) {
                    return TOKEN_BOOL;
                }
                printf("Error: Logical operations require boolean operands\n");
                return -1;
            }
            
            printf("Error: Unknown binary operator\n");
            return -1;
        }
        
        default:
            printf("Error: Unknown expression type\n");
            return -1;
    }
}


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
        case AST_PROGRAM: {
            // Analyze all statements in the program
            for(int i = 0; i < tree->data.program.statement_count; i++){
                analyze_AST(tree->data.program.statements[i], symbol_table);
            }
            break;
        }
        
        case AST_VARIABLE_DECLARATION:{
            // Check for redeclaration
            if(!add_symbol(symbol_table, tree->data.var_declaration.name , tree->data.var_declaration.var_type)){
                if(symbol_table->count >= MAX_SYMBOLS){
                    printf("Error: Symbol table is full\n");
                    return;
                }
                printf("Error: Variable '%s' already declared\n", tree->data.var_declaration.name);
                return;
            }

            // Type check the initializer if present
            if(tree->data.var_declaration.value != NULL){
                enum TokenType expr_type = get_expression_type(tree->data.var_declaration.value, symbol_table);
                if(expr_type == -1){
                    printf("Error: Invalid expression in variable declaration\n");
                    return;
                }
                if(expr_type != tree->data.var_declaration.var_type){
                    printf("Error: Type mismatch in declaration of '%s'. Cannot assign %s to %s\n",
                           tree->data.var_declaration.name,
                           get_token_name(expr_type),
                           get_token_name(tree->data.var_declaration.var_type));
                    return;
                }
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
            // Check type compatibility of binary operation
            enum TokenType result_type = get_expression_type(tree, symbol_table);
            if(result_type == -1){
                printf("Error: Type mismatch in binary operation\n");
                return;
            }
            
            // Recursively analyze operands
            analyze_AST(tree->data.bin_op.left, symbol_table);
            analyze_AST(tree->data.bin_op.right, symbol_table);
            break;
        }
    }
}