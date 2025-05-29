#ifndef SPADE_PARSER_H
#define _SPADE_PARSER_H

typedef enum {
    AST_VARIABLE_DECLARATION,
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BOOLEAN,
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
        
        // null doesn't need data
    } data;
} ASTNode;

#endif