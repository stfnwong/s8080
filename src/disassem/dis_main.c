/* 
 * DIS_MAIN
 * Entry point for disassembler 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "disassem.h"



int main(int argc, char *argv[])
{
    FILE *fp;

    fp = fopen(argv[1], "rb");
    if(fp == NULL)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        exit(1);
    }

    // Get the file size and read file into a buffer
    fseek(fp, 0L, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char* buffer = malloc(fsize);
    if(buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        fclose(fp);
        exit(1);
    }

    fread(buffer, fsize, 1, fp);
    fclose(fp);

    int pc = 0;
    while(pc < fsize)
    {
        pc += disassemble_8080_op(buffer, pc);
    }

    free(buffer);

    return 0;
}
