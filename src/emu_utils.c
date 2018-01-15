/* EMU UTILS 
 * Extra utilities for running/debugging emulator 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "emu_utils.h"


void PrintState(State8080 *state)
{
    fprintf(stdout, "\t");
    // print status flags in a row 
    fprintf(stdout, "%c", state->cc.z  ? 'z' : '.');
    fprintf(stdout, "%c", state->cc.s  ? 's' : '.');
    fprintf(stdout, "%c", state->cc.p  ? 'p' : '.');
    fprintf(stdout, "%c", state->cc.cy ? 'c' : '.');
    fprintf(stdout, "%c", state->cc.ac ? 'a' : '.');
    fprintf(stdout, "\n");
    // Print register contents 
    fprintf(stdout, "A: %02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X\n", state->a,
         state->b,
         state->c,
         state->d,
         state->e,
         state->h,
         state->l);
    // stack pointer 
    fprintf(stdout, "SP:%04X\n", state->sp);
}

void ReadFileToMemory(State8080 *state, const char *filename, int offset)
{
    FILE *fp;
    int num_bytes;
    uint8_t *buffer;

    fp = fopen(filename, "rb");
    if(!fp)
    {
        fprintf(stderr, "Failed to read file %s, exiting\n", filename);
        exit(-1);
    }

    fseek(fp, 0, SEEK_END);
    num_bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    // Load file into memory 
    buffer = &state->memory[offset];
    fread(buffer, num_bytes, 1, fp);
    fclose(fp);
}
