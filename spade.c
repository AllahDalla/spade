#include <stdio.h>
#include "spade.lexer.h"

#define MAX_TOKENS 1024
Token token_array[MAX_TOKENS];
int token_count = 0;

void tokenize_file(char *filename){
    token_count = lexer(filename, token_array, MAX_TOKENS);
    printf("Token 1 : %d, %s\n", token_array[0].type, token_array[0].value);
    printf("Token Count: %d\n", token_count);
    free_tokens(token_array, token_count);
}


int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Usage: %s <filename>/<path/to/file>\n", argv[0]);
        return 1;
    }

    tokenize_file(argv[1]);


    return 0;
}