/* EMU UTILS 
 * Extra utilities for running/debugging emulator 
 *
 */

#ifndef __EMU_UTILS
#define __EMU_UTILS

#include "emu_shell.h"

void PrintState(State8080 *state);
void ReadFileToMemory(State8080 *state, const char *filename, int offset);

#endif /*__EMU_UTILS*/
