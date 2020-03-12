/* EMU8080
 *
 * Entry point for emulator shell
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

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

    int status = 0;
    unsigned long int num_cycles = 0;
    while(status == 0)
    { 
        status = cpu_exec(emu_state);
        if(status < 0)          // trap an unimplmented instruction
            break;
        num_cycles++;
        if(num_cycles > TEST_CYCLE_LIMIT)
        {
            fprintf(stdout, "Hit max cycles (%d)\n", TEST_CYCLE_LIMIT);
            break;
        }
        fprintf(stdout, "[I %04X]  ", emu_state->memory[emu_state->pc]);
        cpu_print_state(emu_state);
    }
    fprintf(stdout, "Emulator finishd with exit code %d\n", status);
    cpu_print_state(emu_state); 

    cpu_destroy(emu_state);

    return 0;
}
