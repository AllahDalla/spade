#include "spade.lexer.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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
    {"boolean", TOKEN_BOOL},
    {"void", TOKEN_VOID},
    
    // Control structures
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {"task", TOKEN_TASK},
    
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
    {"/", TOKEN_DIVIDE},
    {"%", TOKEN_MODULO},
    {"==", TOKEN_EQUALS},
    {"!=", TOKEN_NOT_EQUALS},
    {"<", TOKEN_LESS_THAN},
    {">", TOKEN_GREATER_THAN},
    {"<=", TOKEN_LESS_THAN_EQUALS},
    {">=", TOKEN_GREATER_THAN_EQUALS},
    {"&&", TOKEN_AND},
    {"||", TOKEN_OR},
    {"!", TOKEN_NOT},
    {"(", TOKEN_LPAREN},
    {")", TOKEN_RPAREN},
    {"{", TOKEN_LBRACE},
    {"}", TOKEN_RBRACE},
    {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},
    {";", TOKEN_SEMICOLON},
    {"()", TOKEN_PAREN},
    {"[]", TOKEN_BRACKET},
    {"{}", TOKEN_BRACE},
    {",", TOKEN_COMMA},
    {"->", TOKEN_ARROW}

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
    "TOKEN_PAREN",
    "TOKEN_LBRACE",
    "TOKEN_RBRACE",
    "TOKEN_BRACE",
    "TOKEN_LBRACKET",
    "TOKEN_RBRACKET",
    "TOKEN_BRACKET",
    "TOKEN_SEMICOLON",
    "TOKEN_COMMA",
    "TOKEN_ARROW",
};

const char *get_token_name(enum TokenType type) {
    if (type >= 0 && type < sizeof(token_type_strings) / sizeof(token_type_strings[0])) {
        return token_type_strings[type];
    }
    return "UNKNOWN_TOKEN";
}

// Helper functions
enum TokenType get_keyword_token(char *identifier) {
    int len = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < len; i++) {
        if (strcmp(identifier, keywords[i].keyword) == 0) {
            return keywords[i].token_type;
        }
    }
    return TOKEN_IDENTIFIER; // Not a keyword, so it's an identifier
}

enum TokenType get_operator_token(char *operator) {
    int len = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < len; i++) {
        if (strcmp(operator, operators[i].keyword) == 0) {
            return operators[i].token_type;
        }
    }

    return -1; // Not found
}


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

void free_tokens(Token *token, int token_count){
    for(int i = 0; i < token_count; i++){
        if(token[i].value != NULL){
            free(token[i].value);
            token[i].value = NULL;
        }
    }
}


int lexer(char *filename, Token *token_array, int max_tokens){
    FILE *file = fopen(filename, "rb");
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
            printf("<%s>: %s\n", get_token_name(tk.type), tk.value);
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
            printf("<TOKEN_NUMBER>: %s\n", buffer);
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
                    (byte == '{' && next_byte == '}') || (byte == '[' && next_byte == ']')){
                    // it's a two character token{

                    buffer[index++] = next_byte;
                    buffer[index] = '\0';
                    Token tk = token_type(buffer, 0);
                    printf("<%s>: %s\n", get_token_name(tk.type), tk.value);
                    if(token_index < max_tokens) token_array[token_index++] = tk;
                    token_count++;
                    continue;
                }else{
                    ungetc(next_byte, file);
                }
            }

            buffer[index] = '\0';
            Token tk = token_type(buffer, 0);
            printf("<%s>: %s\n", get_token_name(tk.type), tk.value);
            if(token_index < max_tokens) token_array[token_index++] = tk;
            token_count++;
        }
    }

    fclose(file);
    return token_count;
}