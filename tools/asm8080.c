/*
 * ASM8080
 * Assembler front-end for 8080 emulator
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


int main(int argc, char *argv[])
{
    int status;

    // get a lexer 
    Lexer* lexer = lexer_create();

    status = lex_read_file(lexer, argv[1]);
    if(status != 0)
    {
        fprintf(stderr, "[%s] failed to read file [%s]\n", __func__, argv[1]);
        return -1;
    }


    // Free lexer memory 
    destroy_lexer(lexer);

    return 0;
}

