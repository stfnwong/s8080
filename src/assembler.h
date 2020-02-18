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

/*
 * Instr
 */
typedef struct
{
    uint16_t addr;
    uint16_t instr;
    uint8_t  size;
}Instr;

Instr* instr_create(void);
void   instr_destroy(Instr* instr);
void   instr_init(Instr* instr);
void   instr_copy(Instr* dst, Instr* src);
void   instr_print(Instr* instr);

/*
 * Instruction buffer
 */
typedef struct
{
    Instr** instr_buf;
    int size;
    int max_size;
} InstrBuffer;

InstrBuffer* instr_buffer_create(int size);
void         instr_buffer_destroy(InstrBuffer* buf);
int          instr_buffer_insert(InstrBuffer* buf, Instr* ins);
Instr*       instr_buffer_get(InstrBuffer* buf, int idx);
int          instr_buffer_full(InstrBuffer* buf);
int          instr_buffer_empty(InstrBuffer* buf);

/*
 * ASSEMBLER
 */
typedef struct
{
    // Instruction buffer   ( TODO : data segment buffer)
    InstrBuffer* instr_buf;
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
