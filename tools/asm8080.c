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

    fprintf(stdout, "[%s] lexer text segment: \n");
    line_info_print(lexer->text_seg);
    fprintf(stdout, "\n");

    // Start lexing the file

    // Take the lexed output and assemble it

    // Free lexer memory 
    lexer_destroy(lexer);

    return 0;
}

