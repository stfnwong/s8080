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
ByteVector* vector_create(int capacity)
{
    ByteVector* vec;

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
void vector_destroy(ByteVector* v)
{
    free(v->data);
    free(v);
}

/*
 * vector_push_back()
 */
void vector_push_back(ByteVector* v, uint8_t* data, int len)
{
    if((v->size + len) >= v->capacity)
    {
        if((v->size + len) > 2 * v->capacity)
            vector_extend(v, v->capacity + len);
        else
            vector_extend(v, v->capacity);
    }

    memcpy(v->data + v->size, data, len);
    v->size += len;
}

/*
 * vector_get()
 */
uint8_t* vector_get(ByteVector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return &v->data[idx];
}

/*
 * vector_get_val()
 */
uint8_t vector_get_val(ByteVector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return v->data[idx];        // copy?
}

/*
 * vector_extend()
 */
void vector_extend(ByteVector* v, int ext_size)
{
    uint8_t* mem;

    mem = malloc(sizeof(uint8_t) * v->capacity + ext_size);
    if(!mem)
    {
        fprintf(stdout, "[%s] failed to alloc %d bytes to extend vector\n", __func__, ext_size);
        return;
    }

    memcpy(mem, v->data, v->size);
    free(v->data);
    v->data = mem;
    v->capacity = v->capacity + ext_size;
}

/*
 * vector_init()
 */
void vector_init(ByteVector* v)
{
    // set the memory to zero and reset the size,
    // but leave the capacity as is
    memset(v->data, 0, v->capacity);
    v->size = 0;
}

/*
 * vector_print()
 */
void vector_print(ByteVector* v)
{

}
