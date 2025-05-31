#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.lexer.h"
#include "spade.parser.h"


const char *ast_type_strings[] = {
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
Token current_token(Parser *parser) {
    if (parser->current < parser->token_count) {
        return parser->tokens[parser->current];
    }
    Token eof = {-1, NULL};  // End of file token
    return eof;
}

void advance(Parser *parser) {
    if (parser->current < parser->token_count) {
        parser->current++;
    }
}

Token previous_token(Parser *parser) {
    return parser->tokens[parser->current - 1];
}


int match(Parser *parser, enum TokenType type) {
    if (current_token(parser).type == type) {
        advance(parser);
        return 1;  // Success
    }
    return 0;  // Failed to match
}

// Check if current token is a type
int is_data_type_token(enum TokenType type) {
    return (type == TOKEN_INT || type == TOKEN_STRING || 
            type == TOKEN_BOOL || type == TOKEN_VOID || 
            type == TOKEN_FLOAT || type == TOKEN_DOUBLE || 
            type == TOKEN_LONG);
}
// frees variable declarations for now
void free_AST(ASTNode *node) {
    if (node == NULL) {
        return;
    }else{
        switch(node->type){
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

// Helper function to print AST (for debugging)
void print_AST(ASTNode *node, int indent) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
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