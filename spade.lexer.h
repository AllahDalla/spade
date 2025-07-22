#ifndef SPADE_LEXER_H
#define SPADE_LEXER_H

/**
 * Defines the types of tokens that can be recognized during lexical analysis.
 *
 * This enumeration covers various token types including:
 * - Primitive data types (INT, LONG, FLOAT, DOUBLE, BOOL, VOID)
 * - Literals (STRING, NUMBER, NULL, TRUE, FALSE)
 * - Control flow keywords (IF, ELSE, WHILE, FOR, RETURN, TASK)
 * - Identifiers
 * - Operators (arithmetic, comparison, logical)
 * - Punctuation and grouping symbols
 */
enum TokenType{
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_INT,
    TOKEN_LONG,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,
    TOKEN_STRING,
    TOKEN_BOOL,
    TOKEN_VOID,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_RETURN,
    TOKEN_TASK,
    TOKEN_NUMBER,
    TOKEN_NULL,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_ASSIGN,
    TOKEN_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_POWER,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN_EQUALS,
    TOKEN_GREATER_THAN_EQUALS,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ARROW,
    TOKEN_STRING_LITERAL,
    TOKEN_STRING_CONCAT,
    TOKEN_EOF
};

/**
 * Represents a token in the lexical analysis process.
 *
 * @param type The type of the token (e.g., identifier, keyword, number)
 * @param value The string value associated with the token
 */
typedef struct {
    enum TokenType type;
    char *value;
} Token;

/**
 * Converts a token type enum to its corresponding string representation.
 *
 * @param type The token type to convert to a string
 * @return A string name representing the given token type
 */
const char *get_token_name(enum TokenType type);

/**
 * Prints the details of a single token to the console.
 *
 * @param token The token to be printed, containing its type and value
 */
void print_token(Token token);

/**
 * Frees memory allocated for an array of tokens.
 *
 * @param token The array of tokens to be freed
 * @param token_count The number of tokens in the array
 */
void free_tokens(Token *token, int token_count);

/**
 * Performs lexical analysis on a given file and populates a token array.
 *
 * @param filename The path to the source file to be lexically analyzed
 * @param token_array A pre-allocated array to store the generated tokens
 * @param max_tokens The maximum number of tokens that can be stored in the token_array
 * @return The number of tokens successfully parsed, or a negative error code if lexing fails
 */
int lexer(char *filename, Token *token_array, int max_tokens);

#endif