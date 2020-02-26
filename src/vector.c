/*
 * VECTOR
 * A container a bit like a vector
 * 
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/*
 * vector_push_back()
 */
void vector_push_back(Vector* v, uint8_t* data, int len)
{
    if((v->size + len) >= v->capacity)
        vector_extend(v);

    memcpy(v->data + v->size, data, len);
    v->size += len;
}

/*
 * vector_pop_back()
 */

/*
 * vector_get()
 */
uint8_t* vector_get(Vector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return &v->data[idx];
}


/*
 * vector_get_val()
 */
uint8_t vector_get_val(Vector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return v->data[idx];        // copy?
}


/*
 * vector_extend()
 */
void vector_extend(Vector* v)
{
    uint8_t* mem;

    mem = malloc(sizeof(uint8_t) * 2 * v->capacity);
    if(!mem)
    {
        fprintf(stdout, "[%s] failed to alloc %d bytes to extend vector\n", __func__, 2 * v->capacity);
        return;
    }

    memcpy(mem, v->data, v->size);
    free(v->data);
    v->data = mem;
    v->capacity = 2 * v->capacity;
}
