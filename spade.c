#include <stdio.h>
#include "spade.lexer.h"
#include "spade.parser.h"
#include "spade.symbol.h"
#include "spade.semantic.h"

#define MAX_TOKENS 1024
Token token_array[MAX_TOKENS];
int token_count = 0;

void tokenize_file(char *filename){
    token_count = lexer(filename, token_array, MAX_TOKENS);
    for (int i = 0; i < token_count; i++)  print_token(token_array[i]);
    printf("Token Count: %d\n", token_count);
}


int main(int argc, char *argv[]){
    
    if(argc < 2){
        printf("Usage: %s <filename>/<path/to/file>\n", argv[0]);
        return 1;
    }
    

    for(int i = 1; i < argc; i++){
        printf("File: %s \n", argv[i]);
        printf("=== LEXER OUTPUT ===\n");
        tokenize_file(argv[i]);
    
        if(token_count < 1){
            printf("Error: No tokens found in file <%s>.\n", argv[i]);
        }
    
        printf("\n=== PARSER OUTPUT ===\n");
        Parser parser = {token_array, 0, token_count};
        ASTNode *root = parse_variable_declaration(&parser);
    
        if(root){
            printf("Successfully parsed variable declaration!\n");
            print_AST(root, 0);
            analyze_AST(root, &global_symbol_table);
            print_symbol_table(&global_symbol_table);
            // Clean up
            free_AST(root);
            free_symbol_table(&global_symbol_table);
        }else{
            printf("Failed to parse variable declaration\n");
        }
        
        free_tokens(token_array, token_count);
    }
    
    return 0;
}