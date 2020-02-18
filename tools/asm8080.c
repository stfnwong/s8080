/*
 * ASM8080
 * Assembler front-end for 8080 emulator
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "lexer.h"


// TODO : 
// - add verbose flag
// - add output filename
// - add option to print output
int main(int argc, char *argv[])
{
    int status;

    // get a lexer 
    Lexer* lexer = lexer_create();
    if(!lexer)
    {
        fprintf(stderr, "Failed to create lexer object\n");
        return -2;
    }

    status = lex_read_file(lexer, argv[1]);
    if(status != 0)
    {
        fprintf(stderr, "[%s] failed to read file [%s]\n", __func__, argv[1]);
        return -1;
    }

    // Start lexing the file
    status = lex_all(lexer);
    if(status < 0)
    {
        fprintf(stderr, "[%s] failed to lex file %s\n",
               __func__, argv[1]);
        return -1;
    }

    // Take the lexed output and assemble it
    Assembler* assem = assembler_create();
    if(!assem)
    {
        fprintf(stderr, "Failed to create assembler object\n");
        return -2;
    }
    status = assembler_set_repr(assem, lexer->source_repr);
    if(status < 0)
    {
        fprintf(stderr, "Failed to set source repr\n");
        return -2;
    }


    // Free lexer memory 
    lexer_destroy(lexer);
    assembler_destroy(assem);

    return 0;
}

