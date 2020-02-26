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
typedef struct Vector Vector;

struct Vector
{
    // I only need bytes for this, but a 'real' implementation
    // would use void* here.
    uint8_t* data;
    int      size;
    int      capacity;
    // Also note that in general we would use size_t here, but
    // I know I won't need that much data for this emulator
};

Vector* vector_create(int capacity);
void    vector_destroy(Vector* v);

#endif /*__S8080_VECTOR_H*/
