/*
 * ASSEMBLER
 * Takes output from the Lexer and assembles it
 *
 * Stefan Wong 2020
 */

#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include <stdint.h>
#include "instr.h"
#include "source.h"
#include "vector.h"


/*
 * ASSEMBLER
 */
// TODO : maybe its simpler to compute the addresses here since we 
// can hard-code the instruction sizes. 

typedef struct Assembler Assembler;

// Assembler structure internals
struct Assembler
{
    InstrVector* instr_buf;
    // Lexer output representation
    SourceInfo* src_repr;
    int cur_line;
    int verbose;
};

Assembler* assembler_create(void);
void assembler_destroy(Assembler* assem);

// Copy repr from memory
void assembler_set_repr(Assembler* assem, SourceInfo* repr);

// Assemble from current repr
int assembler_assem_line(Assembler* assem, LineInfo* line);
int assembler_assem(Assembler* assem);

// Getters 
InstrVector* assembler_get_instr_vector(Assembler* assem);

// Misc
void assembler_set_verbose(Assembler* assem);
void assembler_clear_verbose(Assembler* assem);
int  assembler_verbose(Assembler* assem);


#endif /*__ASSEMBLER_H*/
