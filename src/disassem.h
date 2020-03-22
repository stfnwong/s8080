/* DISASSEM
 *
 * Actually not much here since this disassembler only
 * has one method.
 */

#ifndef __DISASSEM_H
#define __DISASSEM_H

#include <stdint.h>
#include "source.h"


// Disassembler struct
typedef struct 
{
    SourceInfo* dis_repr;
    int pc;
    uint8_t* codebuffer;
} Disassembler;

Disassembler* disassembler_create(void);
void disassembler_destroy(Disassembler* dis);

// TODO : try to create something that can return a SourceInfo from 
// a binary
int disassemble_op(uint8_t* codebuffer, int pc);

// Old op function (printf() based)
int disassemble_8080_op_to_console(uint8_t *codebuffer, int pc);

#endif /*__DISASSEM_H*/
