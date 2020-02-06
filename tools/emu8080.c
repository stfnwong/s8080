/* EMU8080
 *
 * Entry point for emulator shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "emu_utils.h"

#define TEST_CYCLE_LIMIT 200000

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

    CPUState *emu_state;
    
    emu_state = cpu_create();
    if(emu_state == NULL)
    {
        fprintf(stderr, "Failed to create state, exiting\n");
        exit(-1);
    }

    fread(emu_state->memory, fsize, 1, fp);
    fclose(fp);

    // Run emulator 
    int status = cpu_run(emu_state, TEST_CYCLE_LIMIT, 1);
    // Print final state
    fprintf(stdout, "Emulator finishd with exit code %d\n", status);
    PrintState(emu_state); 

    cpu_destroy(emu_state);

    return 0;
}
