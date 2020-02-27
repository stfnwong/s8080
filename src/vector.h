/*
 * VECTOR
 * A container a bit like a vector
 * 
 * Stefan Wong 2020
 */

#ifndef __S8080_VECTOR_H
#define __S8080_VECTOR_H

#include <stdint.h>

// TODO : move the definition out of the header
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

ByteVector* vector_create(int capacity);
void        vector_destroy(ByteVector* v);
void        vector_push_back(ByteVector* v, uint8_t* data, int len);
uint8_t*    vector_get(ByteVector* v, int idx);
uint8_t     vector_get_val(ByteVector* v, int idx);
void        vector_extend(ByteVector* v, int ext_size);
void        vector_init(ByteVector* v);
void        vector_print(ByteVector* v);

#endif /*__S8080_VECTOR_H*/
