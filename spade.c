#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "spade.lexer.h"
#include "spade.parser.h"
#include "spade.symbol.h"
#include "spade.semantic.h"
#include "spade.ir.h"
#include "spade.vm.h"

#define MAX_TOKENS 1024
Token token_array[MAX_TOKENS];
int token_count = 0;

/**
 * Tokenizes a source file and outputs token information for debugging.
 * 
 * This function performs lexical analysis on the specified file and prints
 * each token along with its type. It also displays the total token count.
 * 
 * @param filename The path to the source file to be tokenized
 */
void tokenize_file(char *filename){
    token_count = lexer(filename, token_array, MAX_TOKENS);
    for (int i = 0; i < token_count; i++)  print_token(token_array[i]);
    printf("Token Count: %d\n", token_count);
}


/**
 * Main entry point of the Spade compiler.
 * 
 * This function orchestrates the entire compilation pipeline for each input file:
 * 1. Lexical analysis (tokenization)
 * 2. Syntax analysis (parsing to AST)
 * 3. Semantic analysis (type checking and symbol validation)
 * 4. Memory cleanup
 * 
 * @param argc The number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return 0 on success, 1 on usage error
 */
int main(int argc, char *argv[]){

    if(argc == 1){
        printf("Spade Compiler REPL - Enter Spade code (type 'exit' to quit)\n");
        printf("Use '\\' at end of line to continue on next line\n");
        char input[4096];
        char line[1024];
        
        while(1){
            printf("spade> ");
            fflush(stdout);
            
            input[0] = '\0';  // Clear input buffer
            
            // Read multi-line input
            while(1){
                if(!fgets(line, sizeof(line), stdin)){
                    goto exit_repl;
                }
                
                // Remove newline
                line[strcspn(line, "\n")] = 0;
                
                // Check for exit
                if(strcmp(line, "exit") == 0){
                    goto exit_repl;
                }
                
                // Check for continuation
                int len = strlen(line);
                if(len > 0 && line[len-1] == '\\'){
                    line[len-1] = ' ';  // Replace \ with space
                    strcat(input, line);
                    printf("spade> ");  // Continuation prompt
                    fflush(stdout);
                } else {
                    strcat(input, line);
                    break;  // End of multi-line input
                }
            }
            
            if(strlen(input) == 0){
                continue;
            }
            
            // Write input to temporary file
            FILE *temp_file = fopen("temp.sp", "w");
            if(!temp_file){
                printf("Error: Could not create temporary file\n");
                continue;
            }
            fprintf(temp_file, "%s", input);
            fclose(temp_file);
            
            // Process the input
            printf("=== LEXER OUTPUT ===\n");
            tokenize_file("temp.sp");
            
            if(token_count < 1){
                printf("Error: No tokens generated\n");
                continue;
            }
            
            printf("\n=== PARSER OUTPUT ===\n");
            Parser parser = {token_array, 0, token_count};
            ASTNode *root = parse_program(&parser);
            
            if(root){
                printf("Successfully parsed program with %d statements!\n", 
                       root->data.program.statement_count);
                print_AST(root, 0);
                analyze_AST(root, &global_symbol_table);
                print_symbol_table(&global_symbol_table);

                // Generate and execute IR
                printf("\n=== VM EXECUTION ===\n");
                VirtualMachine vm = createVirtualMachine();
                
                printf("\n=== IR GENERATION ===\n");
                IRCode *ir_code = create_ir_code();
                generate_ir(root, ir_code, &global_symbol_table);
                emit_instruction(ir_code, IR_HALT);  // End marker
                print_ir_code(ir_code);
                
                if (vm.machine_state == ERROR) {
                    printf("Error: Failed to create virtual machine\n");
                } else {
                    VMResult result = execute_ir_code(&vm, ir_code);
                    if (result != VM_SUCCESS) {
                        printf("Error executing program: %d\n", result);
                    } else {
                        printf("Program executed successfully!\n");
                    }
                    print_VM_state(&vm);
                }
                
                // Cleanup
                free_AST(root);
                free_ir_code(ir_code);
                free_VM(&vm);
            } else {
                printf("Parse error\n");
            }
            
            printf("\n");
        }
        
        exit_repl:
        // Clean up temp file
        remove("temp.sp");
        return 0;
    }
    
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
        ASTNode *root = parse_program(&parser);
    
        if(root){
            printf("Successfully parsed program with %d statements!\n", 
                   root->data.program.statement_count);
            print_AST(root, 0);
            analyze_AST(root, &global_symbol_table);
            print_symbol_table(&global_symbol_table);

            // NEW: Execute IR code on Virtual Machine
            printf("\n=== VM EXECUTION ===\n");
            VirtualMachine vm = createVirtualMachine();
            
            // NEW: Generate IR code
            printf("\n=== IR GENERATION ===\n");
            IRCode *ir_code = create_ir_code();
            generate_ir(root, ir_code, &global_symbol_table);
            emit_instruction(ir_code, IR_HALT);  // End marker
            print_ir_code(ir_code);
            if (vm.machine_state == ERROR) {
                printf("Error: Failed to create virtual machine\n");
            } else {
                VMResult result = execute_ir_code(&vm, ir_code);
                if (result == VM_SUCCESS) {
                    printf("Program executed successfully!\n");
                    print_VM_state(&vm);
                } else {
                    printf("Error executing program: %d\n", result);
                }
                free_VM(&vm);
            }

            // Clean up
            free_AST(root);
            free_symbol_table(&global_symbol_table);
            free_ir_code(ir_code);
        }else{
            printf("Failed to parse program\n");
        }
        
        free_tokens(token_array, token_count);
    }
    
    return 0;
}