#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "spade.lexer.h"

typedef struct {
    const char *keyword;
    enum TokenType token_type;
} KeywordMap;

const KeywordMap keywords[] = {
    // Data types
    {"int", TOKEN_INT},
    {"long", TOKEN_LONG},
    {"float", TOKEN_FLOAT},
    {"double", TOKEN_DOUBLE},
    {"string", TOKEN_STRING},
    {"bool", TOKEN_BOOL},
    {"void", TOKEN_VOID},
    
    // Control structures
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {"task", TOKEN_TASK},
    {"and", TOKEN_AND},
    {"or", TOKEN_OR},
    
    // Boolean literals
    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},
    
    // Literals
    {"null", TOKEN_NULL}
};

const KeywordMap operators[] = {
    {"=", TOKEN_ASSIGN},
    {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},
    {"*", TOKEN_MULTIPLY},
    {"**", TOKEN_POWER},
    {"/", TOKEN_DIVIDE},
    {"%", TOKEN_MODULO},
    {"==", TOKEN_EQUALS},
    {"!=", TOKEN_NOT_EQUALS},
    {"<", TOKEN_LESS_THAN},
    {">", TOKEN_GREATER_THAN},
    {"<=", TOKEN_LESS_THAN_EQUALS},
    {">=", TOKEN_GREATER_THAN_EQUALS},

    {"!", TOKEN_NOT},
    {"(", TOKEN_LPAREN},
    {")", TOKEN_RPAREN},
    {"{", TOKEN_LBRACE},
    {"}", TOKEN_RBRACE},
    {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},
    {";", TOKEN_SEMICOLON},
    {",", TOKEN_COMMA},
    {"->", TOKEN_ARROW},
};

const char *token_type_strings[] = {
    "TOKEN_IDENTIFIER",
    "TOKEN_KEYWORD",
    "TOKEN_INT",
    "TOKEN_LONG",
    "TOKEN_FLOAT",
    "TOKEN_DOUBLE",
    "TOKEN_STRING",
    "TOKEN_BOOL",
    "TOKEN_VOID",
    "TOKEN_IF",
    "TOKEN_ELSE",
    "TOKEN_WHILE",
    "TOKEN_FOR",
    "TOKEN_RETURN",
    "TOKEN_TASK",
    "TOKEN_NUMBER",
    "TOKEN_NULL",
    "TOKEN_TRUE",
    "TOKEN_FALSE",
    "TOKEN_ASSIGN",
    "TOKEN_EQUALS",
    "TOKEN_NOT_EQUALS",
    "TOKEN_PLUS",
    "TOKEN_MINUS",
    "TOKEN_MULTIPLY",
    "TOKEN_POWER",
    "TOKEN_DIVIDE",
    "TOKEN_MODULO",
    "TOKEN_LESS_THAN",
    "TOKEN_GREATER_THAN",
    "TOKEN_LESS_THAN_EQUALS",
    "TOKEN_GREATER_THAN_EQUALS",
    "TOKEN_AND",
    "TOKEN_OR",
    "TOKEN_NOT",
    "TOKEN_LPAREN",
    "TOKEN_RPAREN",
    "TOKEN_LBRACE",
    "TOKEN_RBRACE",
    "TOKEN_LBRACKET",
    "TOKEN_RBRACKET",
    "TOKEN_SEMICOLON",
    "TOKEN_COMMA",
    "TOKEN_ARROW",
    "TOKEN_STRING_LITERAL",
    "TOKEN_STRING_CONCAT"
};

// Helper functions

/**
 * Converts a token type enum to its corresponding string representation.
 * 
 * @param type The token type to convert to a string
 * @return A string name representing the given token type, or "UNKNOWN_TOKEN" if invalid
 */
const char *get_token_name(enum TokenType type) {
    if (type >= 0 && type < sizeof(token_type_strings) / sizeof(token_type_strings[0])) {
        return token_type_strings[type];
    }
    return "UNKNOWN_TOKEN";
}


/**
 * Prints the details of a single token to the console.
 * 
 * @param token The token to be printed, containing its type and value
 */
void print_token(Token token) {
    printf("[Token] Value: %s \t Type: %s\n", token.value, get_token_name(token.type));
}

/**
 * Determines if an identifier string matches a language keyword.
 * 
 * @param identifier The string to check against the keyword list
 * @return The corresponding TokenType if it's a keyword, TOKEN_IDENTIFIER otherwise
 */
int get_keyword_token(char *identifier) {
    int len = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < len; i++) {
        if (strcmp(identifier, keywords[i].keyword) == 0) {
            return keywords[i].token_type;
        }
    }
    return TOKEN_IDENTIFIER; // Not a keyword, so it's an identifier
}

/**
 * Determines if a string matches a language operator.
 * 
 * @param operator The string to check against the operator list
 * @return The corresponding TokenType if it's an operator, -1 if not found
 */
int get_operator_token(char *operator) {
    int len = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < len; i++) {
        if (strcmp(operator, operators[i].keyword) == 0) {
            return operators[i].token_type;
        }
    }

    return -1; // Not found
}


/**
 * Creates a token from an identifier string based on whether it's a word or operator.
 * 
 * @param identifier The string value for the token
 * @param is_word 1 if the identifier is a word (keyword/identifier), 0 if it's an operator
 * @return A Token structure with the appropriate type and value
 */
Token token_type(char *identifier, int is_word){
    Token tk;
    if(is_word){
        tk.type = get_keyword_token(identifier);
        tk.value = strdup(identifier);
    }else{
        tk.type = get_operator_token(identifier);
        tk.value = strdup(identifier);
    }

    return tk;
}

/**
 * Frees memory allocated for an array of tokens.
 * 
 * @param token The array of tokens to be freed
 * @param token_count The number of tokens in the array
 */
void free_tokens(Token *token, int token_count){
    for(int i = 0; i < token_count; i++){
        if(token[i].value != NULL){
            free(token[i].value);
            token[i].value = NULL;
        }
    }
}


/**
 * Performs lexical analysis on a given file and populates a token array.
 * 
 * This function reads a source file character by character and converts it into
 * a sequence of tokens. It handles identifiers, keywords, numbers, operators,
 * punctuation, and string literals.
 * 
 * @param filename The path to the source file to be lexically analyzed
 * @param token_array A pre-allocated array to store the generated tokens
 * @param max_tokens The maximum number of tokens that can be stored in the token_array
 * @return The number of tokens successfully parsed, or 0 if file cannot be opened
 */
int lexer(char *filename, Token *token_array, int max_tokens){
    FILE *file = fopen(filename, "rb");
    if(!file){
        printf("Error opening file: %s\n", filename);
        return 0;
    }
    int byte;
    int token_index = 0;
    int token_count = 0;
    while((byte = fgetc(file)) != EOF){
        if(isspace(byte)){
            continue; // skip if whitespace
        }else if(isalpha(byte) || byte == '_'){ 
            // identifier
            char buffer[256];
            int index = 0;
            buffer[index++] = byte;
            while((byte = fgetc(file)) != EOF && (isalnum(byte) || byte == '_')){
                if(index < 256) buffer[index++] = byte;
            }

            buffer[index] = '\0';
            Token tk = token_type(buffer, 1);
            if(token_index < max_tokens) token_array[token_index++] = tk;
            token_count++;
            if(byte != EOF) ungetc(byte, file);

        }else if(isdigit(byte)){
            // numbers
            char buffer[256];
            int index = 0;
            buffer[index++] = byte;

            while((byte = fgetc(file)) != EOF && isdigit(byte)){
                if(index < 256) buffer[index++] = byte;
            }
            buffer[index] = '\0';
            // create a token for number TODO: handle floats, doubles, etc.
            Token tk;
            tk.type = TOKEN_NUMBER;
            tk.value = strdup(buffer);
            if(token_index < max_tokens) token_array[token_index++] = tk;
            token_count++;
            if(byte != EOF) ungetc(byte, file);
        }else if(ispunct(byte)){
            // punctuation
            char buffer[256];
            int index = 0;
            buffer[index++] = byte;
            int next_byte = fgetc(file);
            if(next_byte != EOF){ // check if there's another character to avoid tokens making combinations that don't exist
                if((byte == '=' && next_byte == '=') || (byte == '!' && next_byte == '=') ||
                    (byte == '<' && next_byte == '=') || (byte == '>' && next_byte == '=') || 
                    (byte == '&' && next_byte == '&') || (byte == '|' && next_byte == '|') || 
                    (byte == '-' && next_byte == '>') || (byte == '(' && next_byte == ')') ||
                    (byte == '{' && next_byte == '}') || (byte == '[' && next_byte == ']') ||
                    (byte == '*' && next_byte == '*')){
                    // it's a two character token{

                    // check for empty brackets
                    if((byte == '(' || byte == '[' || byte == '{') && (next_byte == ')' || next_byte == ']' || next_byte == '}')){
                        buffer[index] = '\0';
                        Token tk = token_type(buffer, 0); // convert first character to token
                        if(token_index < max_tokens) token_array[token_index++] = tk; // add token to array
                        token_count++;
                        index = 0; // reset index to clear buffer
                        buffer[index++] = next_byte;
                        buffer[index] = '\0';
                        tk = token_type(buffer, 0); // convert second character to token
                        if(token_index < max_tokens) token_array[token_index++] = tk; // add token to array
                        token_count++;
                        continue;
                    
                    }else{

                        buffer[index++] = next_byte;
                        buffer[index] = '\0';
                        Token tk = token_type(buffer, 0);
                        if(token_index < max_tokens) token_array[token_index++] = tk;
                        token_count++;
                        continue;
                    }

                }else{
                    ungetc(next_byte, file);
                }
            }

            if(byte == '"'){
                index = 0;
                // check for string literal
                while((byte = fgetc(file)) != EOF && byte != '"'){
                    if(index < 256) buffer[index++] = byte;
                }
                buffer[index] = '\0';
                Token tk;
                tk.type = TOKEN_STRING_LITERAL;
                tk.value = strdup(buffer);
                if(token_index < max_tokens) token_array[token_index++] = tk;
                token_count++;
                continue;
            }

            // check for comments
            if(byte == '/' && next_byte == '/'){
                while ((byte = fgetc(file)) != '\n'){
                    /* code */
                    // buffer[0] = byte;
                    // buffer[1] = '\0';
                    continue; // consume  all characters until newline
                }

                continue;
            }

            buffer[index] = '\0';
            Token tk = token_type(buffer, 0);
            if(token_index < max_tokens) token_array[token_index++] = tk;
            token_count++;
        }
    }

    Token tk = {TOKEN_EOF, "EOF"};
    if(token_index < max_tokens) token_array[token_index++] = tk;

    fclose(file);
    return token_count;
}