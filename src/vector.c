/*
 * VECTOR
 * A container a bit like a vector
 * 
 * Stefan Wong 2020
 */

#include <stdlib.h>
#include "vector.h"

/*
 * vector_create()
 */
Vector* vector_create(int capacity)
{
    Vector* vec;

    vec = malloc(sizeof(*vec));
    if(!vec)
        return NULL;

    vec->capacity = capacity;
    vec->data = malloc(sizeof(uint8_t) * vec->capacity);
    if(!vec->data)
    {
        free(vec);
        return NULL;
    }

    return vec;
}

/*
 * vector_destroy()
 */
void vector_destroy(Vector* v)
{
    free(v->data);
    free(v);
}
