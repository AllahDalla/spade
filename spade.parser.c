#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.lexer.h"
#include "spade.parser.h"
#include "spade.symbol.h"


SymbolTable global_symbol_table = {0};


const char *ast_type_strings[] = {
    "AST_PROGRAM",
    "AST_VARIABLE_DECLARATION",
    "AST_NUMBER",
    "AST_IDENTIFIER",
    "AST_BOOLEAN",
    "AST_STRING_LITERAL",
    "AST_BINARY_OPERATION",
    "AST_UNARY_OPERATION",
    "AST_NULL"
};

// Helper functions

/**
 * Gets the current token from the parser without advancing.
 * 
 * @param parser The parser instance containing tokens and position
 * @return The current token, or an EOF token if at the end
 */
Token current_token(Parser *parser) {
    if (parser->current < parser->token_count) {
        return parser->tokens[parser->current];
    }
    Token eof = {-1, NULL};  // End of file token
    return eof;
}

/**
 * Advances the parser to the next token.
 * 
 * @param parser The parser instance to advance
 */
void advance(Parser *parser) {
    if (parser->current < parser->token_count) {
        parser->current++;
    }
}

/**
 * Gets the previous token from the parser.
 * 
 * @param parser The parser instance
 * @return The token at the previous position
 */
Token previous_token(Parser *parser) {
    return parser->tokens[parser->current - 1];
}


/**
 * Checks if the current token matches the expected type and advances if it does.
 * 
 * @param parser The parser instance
 * @param type The expected token type
 * @return 1 if the token matches and parser advances, 0 otherwise
 */
int match(Parser *parser, enum TokenType type) {
    if (current_token(parser).type == type) {
        advance(parser);
        return 1;  // Success
    }
    return 0;  // Failed to match
}

/**
 * Checks if a token type represents a data type.
 * 
 * @param type The token type to check
 * @return 1 if the token represents a data type, 0 otherwise
 */
int is_data_type_token(enum TokenType type) {
    return (type == TOKEN_INT || type == TOKEN_STRING || 
            type == TOKEN_BOOL || type == TOKEN_VOID || 
            type == TOKEN_FLOAT || type == TOKEN_DOUBLE || 
            type == TOKEN_LONG);
}
/**
 * Recursively frees memory allocated for an Abstract Syntax Tree node and its children.
 * 
 * @param node The AST node to be freed
 */
void free_AST(ASTNode *node) {
    if (node == NULL) {
        return;
    }else{
        switch(node->type){
            case AST_PROGRAM: {
                for(int i = 0; i < node->data.program.statement_count; i++){
                    free_AST(node->data.program.statements[i]);
                }
                free(node->data.program.statements);
                break;
            }
            
            case AST_VARIABLE_DECLARATION:
                if(node->data.var_declaration.name != NULL){
                    free(node->data.var_declaration.name);
                }

                if(node->data.var_declaration.value != NULL){
                    free_AST(node->data.var_declaration.value);
                }
                break;
            case AST_IDENTIFIER: {
                if(node->data.identifier.name != NULL){
                    free(node->data.identifier.name);
                }
                break;
            }

            case AST_STRING_LITERAL: {
                if(node->data.string_lit.value != NULL){
                    free(node->data.string_lit.value);
                }
                break;
            }

            case AST_NUMBER: break;

            case AST_BOOLEAN: break;

            case AST_BINARY_OPERATION: {
                if(node->data.bin_op.left != NULL){
                    free_AST(node->data.bin_op.left);
                }

                if(node->data.bin_op.right != NULL){
                    free_AST(node->data.bin_op.right);
                }

                break;
            }

            case AST_UNARY_OPERATION: {
                if(node->data.unary_op.operand != NULL){
                    free_AST(node->data.unary_op.operand);
                }
                break;
            }

            case AST_NULL: break;

            default:
                printf("Unable to free AST node - Unknown AST node type - %s\n", ast_type_strings[node->type]);
            
        }

        free(node);
    }
}

/**
 * Recursively prints an Abstract Syntax Tree for debugging purposes.
 * 
 * @param node The AST node to print
 * @param indent The current indentation level for formatting
 */
void print_AST(ASTNode *node, int indent) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("PROGRAM: %d statements\n", node->data.program.statement_count);
            for (int i = 0; i < node->data.program.statement_count; i++) {
                print_AST(node->data.program.statements[i], indent + 1);
            }
            break;
            
        case AST_VARIABLE_DECLARATION:
            printf("VAR_DECL: type=%s, name='%s'\n", 
                   get_token_name(node->data.var_declaration.var_type),
                   node->data.var_declaration.name);
            if (node->data.var_declaration.value) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("value:\n");
                print_AST(node->data.var_declaration.value, indent + 2);
            }
            break;
            
        case AST_NUMBER:
            printf("NUMBER: %d\n", node->data.number.value);
            break;
            
        case AST_IDENTIFIER:
            printf("IDENTIFIER: '%s'\n", node->data.identifier.name);
            break;

        case AST_STRING_LITERAL:
            printf("STRING_LITERAL: '%s'\n", node->data.string_lit.value);
            break;
            
        case AST_BOOLEAN:
            printf("BOOLEAN: %s\n", node->data.boolean.value ? "true" : "false");
            break;

        case AST_BINARY_OPERATION: {
            printf("BINARY_OPERATION: op=%s\n", get_token_name(node->data.bin_op.op));
            if(node->data.bin_op.left){
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("left:\n");
                print_AST(node->data.bin_op.left, indent + 2);
            }
            if(node->data.bin_op.right){
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("right:\n");
                print_AST(node->data.bin_op.right, indent + 2);
            }
            break;

        }

        case AST_UNARY_OPERATION: {
            printf("UNARY_OPERATION: op=%s\n", get_token_name(node->data.unary_op.op));
            if(node->data.unary_op.operand){
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("operand:\n");
                print_AST(node->data.unary_op.operand, indent + 2);
            }
            break;
        }
            
        case AST_NULL:
            printf("NULL\n");
            break;
            
        default:
            printf("UNKNOWN NODE TYPE: %d\n", node->type);
    }
}


// AST construction functions


// ORDER OF OPERATIONS

/*
parse_expression (lowest precedence, e.g. assignment)
parse_equality (==, !=)
parse_comparison (<, >, <=, >=)
parse_term (+, -)
parse_factor (*, /, %)
parse_unary (-, !)
parse_primary (numbers, identifiers, parentheses)
*/

/**
 * Parses a complete program consisting of multiple statements.
 * 
 * Continues parsing statements until end-of-file is reached. Creates a program
 * AST node that contains all parsed statements as children.
 * 
 * @param parser The parser instance
 * @return An AST node representing the complete program, or NULL on error
 */
ASTNode *parse_program(Parser *parser) {
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = AST_PROGRAM;
    program->data.program.capacity = 10;
    program->data.program.statement_count = 0;
    program->data.program.statements = malloc(sizeof(ASTNode*) * program->data.program.capacity);
    
    // Parse statements until end of file
    while (current_token(parser).type != -1) {  // -1 is EOF token
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            printf("Error parsing statement\n");
            free_AST(program);
            return NULL;
        }
        
        // Resize array if needed
        if (program->data.program.statement_count >= program->data.program.capacity) {
            program->data.program.capacity *= 2;
            program->data.program.statements = realloc(program->data.program.statements, 
                sizeof(ASTNode*) * program->data.program.capacity);
        }
        
        program->data.program.statements[program->data.program.statement_count++] = stmt;
    }
    
    return program;
}

/**
 * Determines the type of statement and delegates to the appropriate parser.
 * 
 * Analyzes the current token to determine what kind of statement is being parsed
 * and calls the corresponding parsing function. Currently supports variable
 * declarations, with framework for adding more statement types.
 * 
 * @param parser The parser instance
 * @return An AST node representing the parsed statement, or NULL on error
 */
ASTNode *parse_statement(Parser *parser) {
    Token token = current_token(parser);
    
    // Variable declaration: int, bool, string, etc.
    if (is_data_type_token(token.type)) {
        return parse_variable_declaration(parser);
    }
    
    // Future: Add more statement types
    // if (token.type == TOKEN_IF) return parse_if_statement(parser);
    // if (token.type == TOKEN_WHILE) return parse_while_statement(parser);
    // if (token.type == TOKEN_IDENTIFIER) return parse_assignment_or_call(parser);
    
    printf("Error: Unknown statement starting with %s\n", token.value);
    return NULL;
}

/**
 * Parses an expression using recursive descent (top-level parsing function).
 * 
 * @param parser The parser instance
 * @return An AST node representing the parsed expression, or NULL on error
 */
ASTNode *parse_expression(Parser *parser){
    return parse_equality(parser);
}

ASTNode *parse_equality(Parser *parser){
    ASTNode *left = parse_comparison(parser);
    if(!left){
        return NULL;
    }
    
    while(match(parser, TOKEN_EQUALS) || match(parser, TOKEN_NOT_EQUALS)){
        enum TokenType operator = previous_token(parser).type;
        ASTNode *right = parse_comparison(parser);
        if(!right){
            free_AST(left);
            return NULL;
        }
        ASTNode *bin_node = malloc(sizeof(ASTNode));
        bin_node->type = AST_BINARY_OPERATION;
        bin_node->data.bin_op.op = operator;
        bin_node->data.bin_op.left = left;
        bin_node->data.bin_op.right = right;
        left = bin_node;
    }

    return left;
}

ASTNode *parse_comparison(Parser *parser){

    ASTNode *left = parse_term(parser);
    if(!left){
        return NULL;
    }

    while(match(parser, TOKEN_LESS_THAN) || match(parser, TOKEN_GREATER_THAN) ||
        match(parser, TOKEN_LESS_THAN_EQUALS) || match(parser, TOKEN_GREATER_THAN_EQUALS)){

        enum TokenType operator = previous_token(parser).type;
        ASTNode *right = parse_term(parser);
        if(!right){
            free_AST(left);
            return NULL;
        }

        ASTNode *bin_node = malloc(sizeof(ASTNode));
        bin_node->type = AST_BINARY_OPERATION;
        bin_node->data.bin_op.op = operator;
        bin_node->data.bin_op.left = left;
        bin_node->data.bin_op.right = right;
        left = bin_node;
    }

    return left;
}

ASTNode *parse_term(Parser *parser){

    ASTNode *left = parse_factor(parser);
    if(!left){
        return NULL;
    }

    while(match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)){
        enum TokenType operator = previous_token(parser).type;
        ASTNode *right = parse_factor(parser);
        if(!right){
            free_AST(left);
            return NULL;
        }

        ASTNode *bin_node = malloc(sizeof(ASTNode));
        bin_node->type = AST_BINARY_OPERATION;
        bin_node->data.bin_op.op = operator;
        bin_node->data.bin_op.left = left;
        bin_node->data.bin_op.right = right;
        left = bin_node;
    }

    return left;
}

ASTNode *parse_factor(Parser *parser){

    ASTNode *left = parse_exponent(parser);
    if(!left){
        return NULL;
    }

    while(1){
        if(match(parser, TOKEN_MULTIPLY) || match(parser, TOKEN_DIVIDE) || match(parser, TOKEN_MODULO)){
            enum TokenType operator = previous_token(parser).type;
            ASTNode *right = parse_factor(parser);
            if(!right){
                free_AST(left);
                return NULL;
            }
    
            ASTNode *bin_node = malloc(sizeof(ASTNode));
            bin_node->type = AST_BINARY_OPERATION;
            bin_node->data.bin_op.op = operator;
            bin_node->data.bin_op.left = left;
            bin_node->data.bin_op.right = right;
            left = bin_node;

        }else if(current_token(parser).type ==  TOKEN_LPAREN){
            enum TokenType operator = previous_token(parser).type;
            ASTNode *right = parse_primary(parser);
            if(!right){
                free_AST(left);
                return NULL;
            }


            ASTNode *bin_node = malloc(sizeof(ASTNode));
            bin_node->type = AST_BINARY_OPERATION;
            bin_node->data.bin_op.op = operator;
            bin_node->data.bin_op.left = left;
            bin_node->data.bin_op.right = right;
            left = bin_node;
        }else{
            break;
        }
    }

    return left;
}

ASTNode *parse_exponent(Parser *parser){
    ASTNode *left = parse_unary(parser);
    if(!left){
        return NULL;
    }

    while(match(parser, TOKEN_POWER)){
        enum TokenType operator = previous_token(parser).type;
        ASTNode *right = parse_exponent(parser);
        if(!right){
            free_AST(left);
            return NULL;
        }

        ASTNode *bin_node = malloc(sizeof(ASTNode));
        bin_node->type = AST_BINARY_OPERATION;
        bin_node->data.bin_op.op = operator;
        bin_node->data.bin_op.left = left;
        bin_node->data.bin_op.right = right;
        left = bin_node;
        
    }

    return left;
}

ASTNode *parse_unary(Parser *parser){

    if(match(parser, TOKEN_MINUS) || match(parser, TOKEN_NOT)){
        enum TokenType operator = previous_token(parser).type;
        ASTNode *operand = parse_primary(parser); 
        if(!operand){
            return NULL;
        }
        ASTNode *unary_node = malloc(sizeof(ASTNode));
        unary_node->type = AST_UNARY_OPERATION;
        unary_node->data.unary_op.op = operator;
        unary_node->data.unary_op.operand = operand;
        return unary_node;
    }

    return parse_primary(parser);
}

ASTNode *parse_primary(Parser *parser){

    switch(current_token(parser).type){
        case TOKEN_NUMBER: {
            ASTNode *node = malloc(sizeof(ASTNode));
            node->type = AST_NUMBER;
            node->data.number.value = atoi(current_token(parser).value);
            advance(parser);
            return node;
        }

        case TOKEN_IDENTIFIER: {
            ASTNode *node = malloc(sizeof(ASTNode));
            node->type = AST_IDENTIFIER;
            node->data.identifier.name = strdup(current_token(parser).value);
            advance(parser);
            return node;
        }

        case TOKEN_STRING_LITERAL: {
            ASTNode *node = malloc(sizeof(ASTNode));
            node->type = AST_STRING_LITERAL;
            node->data.string_lit.value = strdup(current_token(parser).value);
            advance(parser);
            return node;
        }

        case TOKEN_TRUE: {
            ASTNode *node = malloc(sizeof(ASTNode));
            node->type = AST_BOOLEAN;
            node->data.boolean.value = 1;
            advance(parser);
            return node;
        }

        case TOKEN_FALSE: {
            ASTNode *node = malloc(sizeof(ASTNode));
            node->type = AST_BOOLEAN;
            node->data.boolean.value = 0;
            advance(parser);
            return node;
        }

        case TOKEN_LPAREN: {
            advance(parser);
            ASTNode *expr = parse_expression(parser);
            if(!expr){
                return NULL;
            }
            if(!match(parser, TOKEN_RPAREN)){
                printf("Error: Expected ')' after expression\n");
                free_AST(expr);
                return NULL;
            }
            return expr;
        }

        default: {
            printf("Error: Unexpeted token '%s'\n", current_token(parser).value);
            return NULL;
        }
    }
}

/**
 * Parses a variable declaration statement.
 * 
 * Handles both variable declarations with and without initialization:
 * - int x;           (declaration only)
 * - int x = 5 + 3;   (declaration with initialization)
 * 
 * @param parser The parser instance
 * @return An AST node representing the variable declaration, or NULL on error
 */
ASTNode *parse_variable_declaration(Parser *parser){
    Token token = current_token(parser);
    
    // check if token starts with a data type
    if(!is_data_type_token(token.type)){
        printf("Error: Expected data type token, got %s\n", token.value);
        return NULL;
    }

    // create variable declaration node
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE_DECLARATION; // set the type of the node
    node->data.var_declaration.var_type = token.type; // set the data type of variable in node
    advance(parser); // advance to the next token


    // check if the next token is an identifier
    token = current_token(parser);
    if(token.type != TOKEN_IDENTIFIER){
        printf("Error: Expected identifier, got %s\n", token.value);
        free_AST(node);
        return NULL;
    }

    node->data.var_declaration.name = strdup(token.value); // set the name of the variable
    advance(parser); // advance to the next token

    // check if its declaration or initialization
    token = current_token(parser);
    if(token.type == TOKEN_ASSIGN){
        // variable is being initialized with a value eg. int x = 5;
        advance(parser); // advance to the next token
        node->data.var_declaration.value = parse_expression(parser); // parse the expression
        if(!node->data.var_declaration.value) {
            printf("Expected expression after '=' \n");
            free_AST(node);
            return NULL;
        }
    }else{
        // variable is being declared without initialization eg. int x;
        node->data.var_declaration.value = NULL;
    }

    if(!match(parser, TOKEN_SEMICOLON)){
        printf("Expected ';' after variable declaration\n");
        free_AST(node);
        return NULL;
    }

    return node;
}