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

            // TESTING STRING CONCATENATION IMPLEMENTATION
            if(expr->data.bin_op.op == TOKEN_PLUS && left_type == TOKEN_STRING && right_type == TOKEN_STRING){
                // should return a proper token indicating string concatenation
                return TOKEN_STRING;
            }
            
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

        case AST_FUNCTION_CALL: {
            char *func_name = expr->data.function_call.name;
            ASTNode *arg_list = expr->data.function_call.arguments;
            int arg_count = arg_list->data.argument_list.argument_count;
            
            // Create Param array (not AST nodes)
            Param *params = malloc(sizeof(Param) * arg_count);
            for(int i = 0; i < arg_count; i++) {
                ASTNode *arg_expr = arg_list->data.argument_list.arguments[i]->data.argument.value;
                params[i].type = get_expression_type(arg_expr, symbol_table);
                params[i].name = NULL;
                
                if(params[i].type == -1) {
                    free(params);
                    return -1;
                }
            }
            
            Symbol *function = lookup_symbol_table_function(symbol_table, func_name, params, arg_count);
            free(params);
            
            if(function != NULL) {
                return function->type;
            }
            
            printf("Error: Function '%s' not found\n", func_name);
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
                if(expr_type != tree->data.var_declaration.var_type && (expr_type != TOKEN_STRING_LITERAL && tree->data.var_declaration.var_type == TOKEN_STRING)){
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

        case AST_FUNCTION_DECLARATION: {
            // Convert AST parameter list to Param array
            ASTNode *param_list_ast = tree->data.function_declaration.parameters;
            int param_count = param_list_ast->data.parameter_list.parameter_count;
            
            Param *params = malloc(sizeof(Param) * param_count);
            for(int i = 0; i < param_count; i++) {
                ASTNode *param_ast = param_list_ast->data.parameter_list.parameters[i];
                params[i].name = strdup(param_ast->data.parameter.name);
                params[i].type = param_ast->data.parameter.type;
            }
            
            if(!add_symbol_function(symbol_table, tree->data.function_declaration.name, 
                                   tree->data.function_declaration.return_type, params, param_count)) {
                printf("Error: Function '%s' already declared\n", tree->data.function_declaration.name);
                // Free allocated memory
                for(int i = 0; i < param_count; i++) {
                    free(params[i].name);
                }
                free(params);
                return;
            }
            
            // Free allocated memory
            for(int i = 0; i < param_count; i++) {
                free(params[i].name);
            }
            free(params);
            
            // For now, just analyze the parameters
            if(tree->data.function_declaration.parameters) {
                analyze_AST(tree->data.function_declaration.parameters, symbol_table);
            }
            if(tree->data.function_declaration.body) {
                analyze_AST(tree->data.function_declaration.body, symbol_table);
            }
            break;
        }

        case AST_PARAMETER_LIST: {
            // Analyze all parameters
            for(int i = 0; i < tree->data.parameter_list.parameter_count; i++){
                analyze_AST(tree->data.parameter_list.parameters[i], symbol_table);
            }
            break;
        }

        case AST_PARAMETER: {
            // TODO: expression type checking
            // parameters are already checked during function declaration and added to local symbol table
            break;
        }


        case AST_FUNCTION_CALL: {
            char *function_name = tree->data.function_call.name;
            ASTNode *arg_list = tree->data.function_call.arguments;
            int arg_count = arg_list->data.argument_list.argument_count;
            
            Param *params = malloc(sizeof(Param) * arg_count);
            for(int i = 0; i < arg_count; i++) {
                ASTNode *arg_expr = arg_list->data.argument_list.arguments[i]->data.argument.value;
                params[i].type = get_expression_type(arg_expr, symbol_table);
                params[i].name = NULL;
                
                if(params[i].type == -1) {
                    printf("Error: Invalid expression as argument in function call\n");
                    free(params);
                    return;
                }
            }
            
            if(lookup_symbol_table_function(symbol_table, function_name, params, arg_count) == NULL) {
                printf("Error: Function '%s' not declared\n", function_name);
                free(params);
                return;
            }
            
            free(params);
            break;
        }

        case AST_NUMBER:
        case AST_BOOLEAN:
        case AST_STRING_LITERAL:
        case AST_NULL:
            // These are leaf nodes, no need to analyze further
            break;

        default:
            printf("Warning: Unknown AST node type in semantic analysis: %d\n", tree->type);
            break;
    }
}