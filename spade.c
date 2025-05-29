#include <stdio.h>
#include "spade.lexer.h"
#include "spade.parser.h"

#define MAX_TOKENS 1024
Token token_array[MAX_TOKENS];
int token_count = 0;

void tokenize_file(char *filename){
    token_count = lexer(filename, token_array, MAX_TOKENS);
    printf("Token 1 : %d, %s\n", token_array[0].type, token_array[0].value);
    printf("Token Count: %d\n", token_count);
}


int main(int argc, char *argv[]){
    
    if(argc < 2){
        printf("Usage: %s <filename>/<path/to/file>\n", argv[0]);
        return 1;
    }
    
    tokenize_file(argv[1]);
    
    printf("=== LEXER OUTPUT ===\n");
    printf("Token Count: %d\n", token_count);

    if(token_count < 1){
        printf("Error: No tokens found in file <%s>.\n", argv[1]);
    }

    printf("\n=== PARSER OUTPUT ===\n");
    Parser parser = {token_array, 0, token_count};
    ASTNode *root = parse_variable_declaration(&parser);

    if(root){
        printf("Successfully parsed variable declaration!\n");
        print_AST(root, 0);
        // Clean up
        free_AST(root);
    }else{
        printf("Failed to parse variable declaration\n");
    }
    
    free_tokens(token_array, token_count);
    return 0;
}