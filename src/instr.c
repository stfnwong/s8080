/*
 * INSTR
 * Instruction structure
 *
 * Stefan Wong 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include "instr.h"

/*
 * instr_create()
 */
Instr* instr_create(void)
{
    Instr* ins;

    ins = malloc(sizeof(*ins));
    if(!ins)
        return NULL;

    ins->addr = 0;
    ins->instr = 0;

    return ins;
}

/*
 * instr_destroy()
 */
void instr_destroy(Instr* ins)
{
    free(ins);
}

/*
 * instr_init()
 */
void instr_init(Instr* instr)
{
    instr->instr = 0;
    instr->addr  = 0;
    instr->size  = 1;
}

/*
 * instr_copy()
 */
void instr_copy(Instr* dst, Instr* src)
{
    if(dst == NULL || src == NULL)
        return;

    dst->addr  = src->addr;
    dst->instr = src->instr;
}

/*
 * instr_print()
 */
void instr_print(Instr* instr)
{
    if(instr->size == 1)
        fprintf(stdout, "[0x%04X] %02X", instr->addr, instr->instr);
    else if(instr->size == 2)
        fprintf(stdout, "[0x%04X] %04X", instr->addr, instr->instr);
    else if(instr->size == 3)
        fprintf(stdout, "[0x%04X] %06X", instr->addr, instr->instr);
    else
        fprintf(stdout, "[%s] invalid instruction size %d\n", __func__, instr->size);
}


// ======== INSTRUCTION BUFFER ======== //
InstrBuffer* instr_buffer_create(int size)
{
    InstrBuffer* buffer;
   
    buffer = malloc(sizeof(*buffer) * size);
    if(!buffer)
        return NULL;

    buffer->size = 0;
    buffer->max_size = size;
    buffer->instr_buf = malloc(sizeof(*buffer->instr_buf) * buffer->max_size);
    if(!buffer->instr_buf)
    {
        return NULL;
    }

    for(int i = 0; i < buffer->max_size; ++i)
    {
        buffer->instr_buf[i] = malloc(sizeof(*buffer->instr_buf[i]));
        if(!buffer->instr_buf[i])
        {
            free(buffer->instr_buf);
            free(buffer);
            return NULL;
        }
        buffer->instr_buf[i] = instr_create();
        if(!buffer->instr_buf[i])
        {
            free(buffer->instr_buf);
            free(buffer);
            return NULL;
        }
    }

    return buffer;
}

/*
 * instr_buffer_destroy()
 */
void  instr_buffer_destroy(InstrBuffer* buf)
{
    if(buf == NULL)
        free(buf);
    else
    {
        for(int i = 0; i < buf->size; ++i)
            instr_destroy(buf->instr_buf[i]);
        free(buf->instr_buf);
        free(buf);
    }
}

/*
 * instr_buffer_insert()
 */
int instr_buffer_insert(InstrBuffer* buf, Instr* ins)
{
    if(buf->size == buf->max_size-1)
        return -1;
    instr_copy(buf->instr_buf[buf->size], ins);
    buf->size++;

    return 0;
}

/*
 * instr_buffer_get()
 */
Instr* instr_buffer_get(InstrBuffer* buf, int idx)
{
    if(idx < 0 || idx > buf->max_size)
        return NULL;

    return buf->instr_buf[idx];
}

/*
 * instr_buffer_full()
 */
int instr_buffer_full(InstrBuffer* buf)
{
    return (buf->size == buf->max_size) ? 1 : 0;
}

/*
 * instr_buffer_empty()
 */
int instr_buffer_empty(InstrBuffer* buf)
{
    return (buf->size == 0) ? 1 : 0;
}
