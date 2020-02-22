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
    int status = 0;

    // get a lexer 
    Lexer* lexer = lexer_create();
    if(!lexer)
    {
        fprintf(stderr, "Failed to create lexer object\n");
        status = -2;
        goto CLEANUP;
    }
    lexer->verbose = 1;     // TODO : remove after debugging complete
    // Take the lexed output and assemble it
    Assembler* assem = assembler_create();
    if(!assem)
    {
        fprintf(stderr, "Failed to create assembler object\n");
        status = -2;
        goto CLEANUP;
    }

    status = lex_read_file(lexer, argv[1]);
    if(status != 0)
    {
        fprintf(stderr, "[%s] failed to read file [%s]\n", __func__, argv[1]);
        status = -1;
        goto CLEANUP;
    }

    // Start lexing the file
    status = lex_all(lexer);
    //if(status < 0)
    //{
    //    fprintf(stderr, "[%s] failed to lex file %s\n",
    //           __func__, argv[1]);
    //    status = -1;
    //    goto CLEANUP;
    //}

    status = assembler_set_repr(assem, lexer->source_repr);
    if(status < 0)
    {
        fprintf(stderr, "Failed to set source repr\n");
        status = -2;
        goto CLEANUP;
    }

    fprintf(stdout, "[%s] lexed %d lines from source file %s\n",
           __func__, lexer->source_repr->size, argv[1]
    );
    for(int l = 0; l < lexer->source_repr->size; ++l)
    {
        LineInfo* cur_line = source_info_get_idx(
                lexer->source_repr, l);
        if(cur_line == NULL)
            continue;
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
    }

    // TODO : start address options, etc
    status = assembler_assem(assem);
    if(status < 0)
    {
        fprintf(stderr, "[%s] failed to assemble file %s\n",
               __func__, argv[1]);
        status = -2;
        goto CLEANUP;
    }

    // Free lexer memory 
    // NOTE : we could also just rely on the OS cleaning
    // everything up on the call to exit()
CLEANUP:
    lexer_destroy(lexer);
    assembler_destroy(assem);

    exit(status);
}

