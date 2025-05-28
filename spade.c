#include <stdio.h>
#include "spade.lexer.h"


int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Usage: %s <filename>/<path/to/file>\n", argv[0]);
        return 1;
    }

    lexer(argv[1]);
    return 0;
}