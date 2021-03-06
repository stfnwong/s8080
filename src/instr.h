/*
 * INSTR
 * Instruction structure
 *
 * Stefan Wong 2020
 */

#ifndef __S8080_INSTR_H
#define __S8080_INSTR_H

#include <stdint.h>

/*
 * Instr
 */
typedef struct
{
    uint32_t instr;
    uint16_t addr;
    uint8_t  size;
}Instr;

// NOTE: This will be an 8 byte structure, probably due to packing. 
// This means that we could re-pack here to save a byte (by putting the 
// size in the upper region of instr), or add another byte with 
// effectively no penalty

Instr* instr_create(void);
void   instr_destroy(Instr* instr);
void   instr_init(Instr* instr);
void   instr_copy(Instr* dst, Instr* src);
void   instr_print(Instr* instr);
//void   instr_print_asm(Instr* instr);       // print instr as assembly (eg: disassemble to console)

// NOTE: more or less deprecated - be sure to remove this before final merge
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

// Display 


#endif /*__S8080_INSTR_H*/
