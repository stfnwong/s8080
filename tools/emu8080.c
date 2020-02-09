/* EMU8080
 *
 * Entry point for emulator shell
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "emu_utils.h"
#include "display.h"


#define TEST_CYCLE_LIMIT 200000


int main(int argc, char *argv[])
{
    int cpu_verbose = 0;            // TODO : add command line option
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

    // Get a new state
    CPUState *emu_state;
    emu_state = cpu_create();
    if(emu_state == NULL)
    {
        fprintf(stderr, "Failed to create state, exiting\n");
        exit(-1);
    }

    // Load the data into memory
    fread(emu_state->memory, fsize, 1, fp);
    fclose(fp);

    // Setup the display
    Display* disp;

    disp = display_create();
    if(disp == NULL)
    {
        fprintf(stderr, "Failed to create display\n");
        exit(-1);
    }

    // Run emulator 
    int status;
    uint32_t last_tic = SDL_GetTicks();     // unit is milliseconds
    uint32_t cur_tic;
    unsigned long int step = 0;

    while(1)
    {
        cur_tic = SDL_GetTicks();
        if((cur_tic - last_tic) >= DISP_TIC)
        {
            last_tic = SDL_GetTicks();
            status   = cpu_run(emu_state, DISP_CYCLES_PER_TIC, cpu_verbose);
            display_draw(disp, emu_state->memory);
            step++;
            if(step > TEST_CYCLE_LIMIT)
                break;
        }
    }
    // Print final state
    fprintf(stdout, "Emulator finishd with exit code %d\n", status);
    PrintState(emu_state); 

    cpu_destroy(emu_state);
    display_destroy(disp);

    return 0;
}
