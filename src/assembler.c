/*
 * ASSEMBLER
 * Takes output from the Lexer and assembles it
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"
#include "opcode.h"


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
    instr->addr = 0;
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
    fprintf(stdout, "[0x%04X] %02X", instr->addr, instr->instr);
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


void  instr_buffer_destroy(InstrBuffer* buf)
{
    for(int i = 0; i < buf->size; ++i)
        instr_destroy(buf->instr_buf[i]);
    free(buf->instr_buf);
    free(buf);
}

/*
 * instr_buffer_insert()
 */
void instr_buffer_insert(InstrBuffer* buf, Instr* ins)
{
    if(buf->size == buf->max_size-1)
        return;
    instr_copy(buf->instr_buf[buf->size], ins);
    buf->size++;
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



// ROUTINES FOR EACH INSTRUCTION
// TODO : Data segment

uint8_t asm_reg_to_code(char reg)
{
    switch(reg)
    {
        case 'A':
            return 0x7;
        case 'B':
            return 0x0;
        case 'C':
            return 0x1;
        case 'D':
            return 0x2;
        case 'E':
            return 0x3;
        case 'H':
            return 0x4;
        case 'L':
            return 0x5;
        case 'M':
            return 0x6;
        default:
            return 0x0;
    }
}


/*
 * asm_arith_instr_to_code()
 */
uint8_t asm_arith_instr_to_code(uint8_t instr)
{
    uint8_t code_out = 0;
    switch(instr)
    {
        case LEX_ADD:
            code_out = code_out | (0x1 << 7);
            break;

        case LEX_ADC:
            code_out = code_out | (0x1 << 3) | (0x1 << 7);
            break;

        case LEX_SUB:
            code_out = code_out | (0x2 << 3) | (0x1 << 7);
            break;

        case LEX_SBB:
            code_out = code_out | (0x3 << 3) | (0x1 << 7);
            break;

        case LEX_ANA:
            code_out = code_out | (0x4 << 3) | (0x1 << 7);
            break;

        case LEX_XRA:
            code_out = code_out | (0x5 << 3) | (0x1 << 7);
            break;

        case LEX_ORA:
            code_out = code_out | (0x6 << 3) | (0x1 << 7);
            break;

        case LEX_CMP:
            code_out = code_out | (0x7 << 3) | (0x1 << 7);
            break;

        case LEX_RLC:
            code_out = code_out | 0x7;
            break;

        case LEX_RRC:
            code_out = code_out | (0x1 << 3) | 0x7;
            break;

        case LEX_RAL:
            code_out = code_out | (0x2 << 3) | 0x7;
            break;

        case LEX_RAR:
            code_out = code_out | (0x3 << 3) | 0x7;
            break;
    }

    return code_out;
}


/*
 * asm_mov_instr_to_code()
 */
uint8_t asm_mov_instr_to_code(uint8_t instr)
{
    uint8_t code_out = 0;

    switch(instr)
    {
        case LEX_PUSH:
            code_out = code_out | (0x3 << 6) | 0x5;
            break;

        case LEX_POP:
            code_out = code_out | (0x3 << 6) | 0x1;
            break;

        case LEX_DAD:
            code_out = code_out | 0x9;
            break;

        case LEX_INX:
            code_out = code_out | 0x3;
            break;

        case LEX_DCX:
            code_out = code_out | 0xB;
            break;

        case LEX_XCHG:
            code_out = 0xEB;
            break;

        case LEX_XTHL:
            code_out = 0xE3;
            break;

        case LEX_SPHL:
            code_out = 0xF9;
            break;
    }

    return code_out;
}

/*
 * asm_8bit_arith()
 */
void asm_8bit_arith(Instr* dst, LineInfo* line)
{
    dst->instr = asm_arith_instr_to_code(line->opcode->instr);
    dst->instr = dst->instr | line->reg[0];
    dst->addr  = line->addr;
}

/*
 * asm_16bit_arith()
 */
void asm_16bit_arith(Instr* dst, LineInfo* line)
{
}

void asm_imm(Instr* dst, LineInfo* line)
{
}



/*
 * assembler_create()
 */
Assembler* assembler_create(void)
{
    Assembler* assem;

    assem = malloc(sizeof(*assem));
    if(!assem)
        goto ASSEM_END;

    assem->src_repr = NULL;
    assem->cur_line = 0;
    assem->verbose  = 0;

    // Set the buffer empty until we have a src_repr
    assem->instr_buf = NULL;

ASSEM_END:
    if(!assem)
    {
        fprintf(stderr, "[%s] failed to create Assembler object\n", __func__);
        return NULL;
    }

    return assem;
}


/*
 * assembler_create_buffer()
 */
Instr** assembler_create_buffer(int size)
{
}

/*
 * assembler_destroy_buffer()
 */
void assembler_destroy_buffer(Assembler* assem)
{
}

/*
 * assembler_destroy()
 */
void assembler_destroy(Assembler* assem)
{
    free(assem);
}

/*
 * assembler_set_repr()
 */
int assembler_set_repr(Assembler* assem, SourceInfo* repr)
{
    assem->src_repr = repr;
    if(assem->instr_buf != NULL)
        assembler_destroy_buffer(assem);

    assem->instr_buf = instr_buffer_create(repr->size);
    if(assem->instr_buf == NULL)
    {
        fprintf(stdout, "[%s] failed to create instruction buffer for assembler object\n", __func__);
        return -1;
    }

    return 0;
}

/*
 * assembler_assem_line()
 */
int assembler_assem_line(Assembler* assem, LineInfo* line)
{
    Instr cur_instr;

    instr_init(&cur_instr);
    if(line->opcode != NULL)
    {
        if(assem->verbose)
        {
            fprintf(stdout, "[%s] assembling instruction %02X [%s]\n",
                    __func__, line->opcode->instr, line->opcode->mnemonic
            );
        }

        switch(line->opcode->instr)
        {
            case LEX_ADC:
            case LEX_ADD:
            case LEX_ANA:
            case LEX_CMP:
            case LEX_ORA:
            case LEX_RAR:
            case LEX_RLC:
            case LEX_RRC:
            case LEX_SBB:
            case LEX_SUB:
                asm_8bit_arith(&cur_instr, line);
                break;

            case LEX_INR:
                //asm_inr(&cur_instr, line);
                break;


            default:
            {
                fprintf(stdout, "[%s] unknown opcode %02X [%s]\n", 
                        __func__, line->opcode->instr, line->opcode->mnemonic
                );
                break;
            }
        }
    }
}


/*
 * assembler_assem()
 */
int assembler_assem(Assembler* assem)
{
    int status;

    if(assem->src_repr == NULL)
    {
        if(assem->verbose)
            fprintf(stdout, "[%s] no src_repr set in assembler\n", __func__);
        return -1;
    }

    for(int l = 0; l < assem->src_repr->size; ++l)
    {
        LineInfo* cur_line = assem->src_repr->buffer[l];
        assembler_assem_line(assem, cur_line);
    }

    return status;
}
