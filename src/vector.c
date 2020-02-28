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
 * byte_vector_create()
 */
ByteVector* byte_vector_create(int capacity)
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
 * byte_vector_destroy()
 */
void byte_vector_destroy(ByteVector* v)
{
    free(v->data);
    free(v);
}

/*
 * byte_vector_push_back()
 */
void byte_vector_push_back(ByteVector* v, uint8_t* data, int len)
{
    if((v->size + len) >= v->capacity)
    {
        if((v->size + len) > 2 * v->capacity)
            byte_vector_extend(v, v->capacity + len);
        else
            byte_vector_extend(v, v->capacity);
    }

    memcpy(v->data + v->size, data, len);
    v->size += len;
}

/*
 * byte_vector_get()
 */
uint8_t* byte_vector_get(ByteVector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return &v->data[idx];
}

/*
 * byte_vector_get_val()
 */
uint8_t byte_vector_get_val(ByteVector* v, int idx)
{
    if(idx < 0 || idx >= v->size)
        return NULL;

    return v->data[idx];        // copy?
}

/*
 * byte_vector_extend()
 */
void byte_vector_extend(ByteVector* v, int ext_size)
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
 * byte_vector_init()
 */
void byte_vector_init(ByteVector* v)
{
    // set the memory to zero and reset the size,
    // but leave the capacity as is
    memset(v->data, 0, v->capacity);
    v->size = 0;
}

/*
 * byte_vector_size()
 */
int byte_vector_size(ByteVector* v)
{
    return v->size;
}

/*
 * byte_vector_capacity()
 */
int byte_vector_capacity(ByteVector* v)
{
    return v->capacity;
}


/*
 * byte_vector_print()
 */
void byte_vector_print(ByteVector* v)
{
    fprintf(stdout, "ByteVector [size: %d, capacity: %d] :\n", 
            v->size, v->capacity);

    for(int b = 0; b < v->size; ++b)
    {
        if((b > 0) && (b % 8 == 0))
            fprintf(stdout, "\n");
        fprintf(stdout, "%02X ", v->data[b]);
    }
    fprintf(stdout, "\n");
}
