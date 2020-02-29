/*
 * VECTOR
 * A container a bit like a vector
 * 
 * Stefan Wong 2020
 */

#ifndef __S8080_VECTOR_H
#define __S8080_VECTOR_H

#include <stdint.h>
#include "instr.h"

typedef struct ByteVector ByteVector;

struct ByteVector
{
    // I only need bytes for this, but a 'real' implementation
    // would use void* here.
    uint8_t* data;
    int      size;
    int      capacity;
    // Also note that in general we would use size_t here, but
    // I know I won't need that much data for this emulator
};

ByteVector* byte_vector_create(int capacity);
void        byte_vector_destroy(ByteVector* v);
void        byte_vector_push_back(ByteVector* v, uint8_t* data, int len);
uint8_t*    byte_vector_get(ByteVector* v, int idx);
uint8_t     byte_vector_get_val(ByteVector* v, int idx);
void        byte_vector_extend(ByteVector* v, int ext_size);
void        byte_vector_init(ByteVector* v);

// info
int         byte_vector_size(ByteVector* v);
int         byte_vector_capacity(ByteVector* v);

// display 
void        byte_vector_print(ByteVector* v);


// Instruction vector?
typedef struct InstrVector InstrVector;

struct InstrVector
{
    Instr* buffer;
    int size;
    int capacity;
};

InstrVector* instr_vector_create(int capacity);
void         instr_vector_destroy(InstrVector* vec);
Instr*       instr_vector_get(InstrVector* vec, int idx);
void         instr_vector_push_back(InstrVector* vec, Instr* instr);
void         instr_vector_extend(InstrVector* vec, int ext_size);





#endif /*__S8080_VECTOR_H*/
