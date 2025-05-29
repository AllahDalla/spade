#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spade.parser.h"
#include "spade.lexer.h"

typedef struct {
    Token *tokens;
    int current;
    int token_count;
} Parser;

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
                    node->data.var_declaration.name = NULL;
                }

                if(node->data.var_declaration.value != NULL){
                    free_AST(node->data.var_declaration.value);
                    node->data.var_declaration.value = NULL;
                }
                break;
            case AST_IDENTIFIER:
                break;
            case AST_NUMBER:
                break;
            case AST_BOOLEAN:
                break;
            case AST_NULL:
                break;
            default:
                printf("Unable to free AST node - Unknown AST node type\n");
            
        }

        free(node);
    }
}


// AST construction functions

ASTNode *parse_expression(Parser *parser){
    return NULL;
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