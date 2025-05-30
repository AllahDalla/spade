#ifndef SPADE_PARSER_H
#define SPADE_PARSER_H

#include "spade.lexer.h"

typedef enum {
    AST_VARIABLE_DECLARATION,
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BOOLEAN,
    AST_STRING_LITERAL,
    AST_BINARY_OPERATION,
    AST_UNARY_OPERATION,
    AST_NULL
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            enum TokenType var_type;    // TOKEN_INT, TOKEN_STRING, etc.
            char *name;                 // Variable name
            struct ASTNode *value;      // NULL if no initialization
        } var_declaration;
        
        struct {
            int value;
        } number;
        
        struct {
            char *name;
        } identifier;
        
        struct {
            int value;  // 1 for true, 0 for false
        } boolean;

        struct {
            char *value;
        } string_lit;

        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            enum TokenType op;
        } bin_op;

        struct {
            enum TokenType op;
            struct ASTNode *operand;
        } unary_op;
        
        // null doesn't need data
    } data;
} ASTNode;

typedef struct {
    Token *tokens;
    int current;
    int token_count;
} Parser;

void print_AST(ASTNode *node, int indent);
void free_AST(ASTNode *node);
ASTNode *parse_expression(Parser *parser);
ASTNode *parse_equality(Parser *parser);
ASTNode *parse_comparison(Parser *parser);
ASTNode *parse_term(Parser *parser);
ASTNode *parse_factor(Parser *parser);
ASTNode *parse_unary(Parser *parser);
ASTNode *parse_primary(Parser *parser);
ASTNode *parse_variable_declaration(Parser *parser);

#endif