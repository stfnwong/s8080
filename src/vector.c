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

    vec->size = 0;
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
        return 0;       // TODO : what to do here?

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


// ======== INSTRVECTOR ========= //
/*
 * instr_vector_create()
 */
InstrVector* instr_vector_create(int capacity)
{
    InstrVector* vec;

    vec = malloc(sizeof(*vec));
    if(!vec)
        return NULL;

    vec->size = 0;
    vec->capacity = capacity;
    vec->buffer = malloc(sizeof(*vec->buffer) * vec->capacity);
    if(!vec->buffer)
    {
        free(vec);
        return NULL;
    }

    return vec;
}


/*
 * instr_vector_destroy()
 */
void instr_vector_destroy(InstrVector* vec)
{
    free(vec->buffer);
    free(vec);
}

/*
 * instr_vector_get()
 */
Instr* instr_vector_get(InstrVector* vec, int idx)
{
    if(idx < 0 || idx >= vec->size)
        return NULL;

    return &vec->buffer[idx];
}

/*
 * instr_vector_push_back()
 */
void instr_vector_push_back(InstrVector* vec, Instr* instr)
{
    if(vec->size >= vec->capacity)
        instr_vector_extend(vec, vec->capacity);

    memcpy(vec->buffer + vec->size, instr, sizeof(*vec->buffer));
    vec->size++;
}

/*
 * instr_vector_extend()
 */
void instr_vector_extend(InstrVector* vec, int ext_size)
{
    Instr* buf;

    buf = malloc(sizeof(*vec->buffer) * (vec->capacity + ext_size));
    if(!buf)
    {
        fprintf(stdout, "[%s] failed to alloc %d bytes to extend vector\n", __func__, ext_size);
        return;
    }

    vec->capacity = vec->capacity + ext_size;
    memcpy(buf, vec->buffer, sizeof(*buf) * vec->size);
    free(vec->buffer);
    vec->buffer = buf;
}

// -------- INFO 
/*
 * intr_vector_size()
 */
int instr_vector_size(InstrVector* vec)
{
    return vec->size;
}

/*
 * intr_vector_capacity()
 */
int instr_vector_capcaity(InstrVector* vec)
{
    return vec->capacity;
}

/*
 * instr_vector_print()
 */
void instr_vector_print(InstrVector* vec)
{
    fprintf(stdout, "size     : %d\n", vec->size);
    fprintf(stdout, "capacity : %d\n", vec->capacity);
    for(int v = 0; v < vec->size; ++v)
    {
        fprintf(stdout, "%5d : ", v);
        instr_print(&vec->buffer[v]);
        fprintf(stdout, "\n");
    }
}
