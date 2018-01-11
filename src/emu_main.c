/* EMU MAIN
 * Entry point for emulator shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "emu_shell.h"


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

    State8080 emu_state; 
    emu_state.pc = 0;
    emu_state.memory = malloc(fsize * sizeof(uint8_t));
    if(emu_state.memory == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for buffer\n");
        fclose(fp);
        exit(1);
    }

    fread(emu_state.memory, fsize, 1, fp);
    fclose(fp);

    int status = 0;
    while(emu_state.pc < fsize)
    { 
        status = Emulate8080(&emu_state);
        if(status < 0)
            break;
    }

    free(emu_state.memory);

    return 0;
}
