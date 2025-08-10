#ifndef SPADE_PARSER_H
#define SPADE_PARSER_H

#include "spade.lexer.h"

typedef enum {
    AST_PROGRAM,              // Container for multiple statements
    AST_VARIABLE_DECLARATION,
    AST_FUNCTION_DECLARATION,
    AST_FUNCTION_CALL,
    AST_PARAMETER_LIST,
    AST_PARAMETER,
    AST_ARGUMENT_LIST,
    AST_ARGUMENT,
    AST_ASSIGNMENT,
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
            struct ASTNode **statements;  // Array of statement pointers
            int statement_count;          // Number of statements
            int capacity;                 // Allocated capacity
        } program;
        
        struct {
            enum TokenType var_type;    // TOKEN_INT, TOKEN_STRING, etc.
            char *name;                 // Variable name
            struct ASTNode *value;      // NULL if no initialization
        } var_declaration;
        
        struct {
            char *name;                       // Function name identifier
            enum TokenType return_type;       // Return type (TOKEN_INT, TOKEN_STRING, etc.)
            struct ASTNode *parameters;       // Pointer to AST_PARAMETER_LIST node
            struct ASTNode *body;             // Function body (currently NULL for empty {})
        }function_declaration;
        
        struct {
            enum TokenType type;              // Parameter type (TOKEN_INT, TOKEN_STRING, etc.)
            char *name;                       // Parameter name identifier
        }parameter;
        
        struct {
            struct ASTNode **parameters;      // Array of AST_PARAMETER node pointers
            int parameter_count;              // Number of parameters in the list
            int capacity;                     // Allocated capacity for parameters array
        }parameter_list;
        
        struct {
            char *name;                       // Function name to call
            struct ASTNode *arguments;        // Pointer to AST_ARGUMENT_LIST node
        }function_call;

        struct {
            struct ASTNode **arguments;       // Array of AST_ARGUMENT node pointers
            int argument_count;               // Number of arguments in the list
            int capacity;                     // Allocated capacity for arguments array
        }argument_list;

        struct {
            enum TokenType type;              // Argument type (determined during semantic analysis)
            struct ASTNode *value;            // Expression value of the argument
        }argument;

        struct {
            char *name;
            struct ASTNode *value;
        }variable_assignment;
        
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
ASTNode *parse_program(Parser *parser);
ASTNode *parse_statement(Parser *parser);
ASTNode *parse_expression(Parser *parser);
ASTNode *parse_logical_or(Parser *parser);
ASTNode *parse_logical_and(Parser *parser);
ASTNode *parse_equality(Parser *parser);
ASTNode *parse_comparison(Parser *parser);
ASTNode *parse_term(Parser *parser);
ASTNode *parse_factor(Parser *parser);
ASTNode *parse_exponent(Parser *parser);
ASTNode *parse_unary(Parser *parser);
ASTNode *parse_primary(Parser *parser);
ASTNode *parse_variable_declaration(Parser *parser);
ASTNode *parse_function_declaration(Parser *parser);
ASTNode *parse_parameter_list(Parser *parser);
ASTNode *parse_assignment(Parser *parser);

#endif