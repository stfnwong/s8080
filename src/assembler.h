/*
 * ASSEMBLER
 * Takes output from the Lexer and assembles it
 *
 * Stefan Wong 2020
 */

#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include <stdint.h>
#include "source.h"

typedef struct
{
    uint16_t addr;
    uint8_t  instr;
}Instr;

void instr_init(Instr* instr);
void instr_print(Instr* instr);


/*
 * ASSEMBLER
 */
typedef struct
{
    // Instruction buffer
    Instr** instr_buf;
    int instr_buf_size;
    // Lexer output representation
    SourceInfo* src_repr;
    int cur_line;
    int verbose;
} Assembler;

Assembler* assembler_create(void);
void assembler_destroy(Assembler* assem);

// Read repr from disk

// Copy repr from memory
int assembler_set_repr(Assembler* assem, SourceInfo* repr);

// Assemble from current repr
int assembler_assem_line(Assembler* assem, LineInfo* line);
int assembler_assem(Assembler* assem);


#endif /*__ASSEMBLER_H*/
