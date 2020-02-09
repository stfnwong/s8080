/* EMU UTILS 
 * Extra utilities for running/debugging emulator 
 *
 */

#ifndef __EMU_UTILS
#define __EMU_UTILS

#include "cpu.h"

void PrintState(CPUState *state);
void ReadFileToMemory(CPUState *state, const char *filename, int offset);

#endif /*__EMU_UTILS*/
