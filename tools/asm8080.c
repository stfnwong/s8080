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
    size_t src_file_size;
    FILE *fp;

    fp = fopen(argv[1], "rb");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        exit(1);
    }

    // Get the file size and read file into a buffer
    fseek(fp, 0L, SEEK_END);
    src_file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char* buffer = malloc(src_file_size);
    if(buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory (%ld bytes) for buffer\n", src_file_size);
        fclose(fp);
        exit(1);
    }

    // get a lexer 
    Lexer* lexer = lexer_create();

    status = lex_read_file(lexer, argv[1]);



    // Free lexer memory 
    destroy_lexer(lexer);

    return 0;
}

