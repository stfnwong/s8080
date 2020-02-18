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
    if(buf->size == buf->max_size)
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

uint8_t asm_pair_reg_to_code(char reg)
{
    switch(reg)
    {
        case 'B':
        case 'C':
            return 0x0;
        case 'D':
        case 'E':
            return 0x1;
        case 'H':
        case 'L':
            return 0x2;
        case 'S':
            return 0x3;
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

void asm_reg_pair(Instr* dst, LineInfo* line, uint8_t op)
{
    dst->instr = op;
    dst->instr = asm_pair_reg_to_code(line->reg[0]) << 4;
    dst->addr  = dst->addr;
}

void asm_imm_2byte(Instr* dst, LineInfo* line, uint8_t op)
{
    dst->instr = (0x3 << 14) | (op << 11) | (0x6 << 8);
    dst->instr = dst->instr | line->immediate;
    dst->size = 2;
    dst->addr = line->addr;
}

void asm_imm_3byte(Instr* dst, LineInfo* line)
{
    dst->instr = 0x1 << 9;
    dst->size = 3;
    dst->addr = line->addr;
}

void asm_lxi(Instr* dst, LineInfo* line)
{

}

void asm_mvi(Instr* dst, LineInfo* line)
{
    dst->instr = (0x6 << 8);
    // TODO : not quite the same code LUT as other instructions
    dst->instr = dst->instr | (asm_reg_to_code(line->reg[0]) << 11);
    dst->instr = dst->instr | line->immediate;
    dst->size  = 2;
    dst->addr  = line->addr;
}

void asm_ldax(Instr* dst, LineInfo* line)
{
    dst->instr = 0x9;
    if(line->reg[0] == 'D')
        dst->instr = dst->instr | (0x1 << 4);
    dst->addr = line->addr;
}

void asm_stax(Instr* dst, LineInfo* line)
{
    dst->instr = 0x1 << 1;
    if(line->reg[0] == 'D')
        dst->instr = dst->instr | (0x1 << 4);
    dst->addr = line->addr;
}


void asm_mov(Instr* dst, LineInfo* line)
{
    dst->instr = 0x1 << 6;
    dst->instr = dst->instr | (asm_reg_to_code(line->reg[0]) << 3);
    dst->instr = dst->instr |  asm_reg_to_code(line->reg[1]);
    dst->addr = line->addr;
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
 * assembler_destroy()
 */
void assembler_destroy(Assembler* assem)
{
    instr_buffer_destroy(assem->instr_buf);
    source_info_destroy(assem->src_repr);
    free(assem);
}

/*
 * assembler_set_repr()
 */
int assembler_set_repr(Assembler* assem, SourceInfo* repr)
{
    assem->src_repr = repr;
    if(assem->instr_buf != NULL)
        instr_buffer_destroy(assem->instr_buf);

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
    int status = 0;
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
            case LEX_XRA:
                asm_8bit_arith(&cur_instr, line);
                break;

            case LEX_ADI:
                asm_imm_2byte(&cur_instr, line, 0x00);
                break;

            case LEX_ACI:
                asm_imm_2byte(&cur_instr, line, 0x01);
                break;

            case LEX_SUI:
                asm_imm_2byte(&cur_instr, line, 0x02);
                break;

            case LEX_SBI:
                asm_imm_2byte(&cur_instr, line, 0x03);
                break;

            case LEX_ANI:
                asm_imm_2byte(&cur_instr, line, 0x04);
                break;

            case LEX_XRI:
                asm_imm_2byte(&cur_instr, line, 0x05);
                break;

            case LEX_ORI:
                asm_imm_2byte(&cur_instr, line, 0x06);
                break;

            case LEX_CPI:
                asm_imm_2byte(&cur_instr, line, 0x07);
                break;

            case LEX_MVI:
                asm_mvi(&cur_instr, line);
                break;

            case LEX_LXI:
                asm_lxi(&cur_instr, line);
                break;

            // pair instructions
            case LEX_DAD:
                asm_reg_pair(&cur_instr, line, 0x9);
                break;

            case LEX_DCX:
                asm_reg_pair(&cur_instr, line, 0xB);
                break;

            case LEX_INX:
                asm_reg_pair(&cur_instr, line, 0x3);
                break;

            case LEX_INR:
                //asm_inr(&cur_instr, line);
                break;

            case LEX_LDAX:
                asm_ldax(&cur_instr, line);
                break;

            case LEX_STAX:
                asm_stax(&cur_instr, line);
                break;

            // Move instructions 
            case LEX_MOV:
                asm_mov(&cur_instr, line);
                break;

            case LEX_POP:
                break;

            case LEX_PUSH:
                break;

            default:
            {
                fprintf(stdout, "[%s] unknown opcode %02X [%s]\n", 
                        __func__, line->opcode->instr, line->opcode->mnemonic
                );
                status = -1;
                break;
            }
        }
    }
    status = instr_buffer_insert(assem->instr_buf, &cur_instr);
    if(status == -1)
    {
        fprintf(stdout, "[%s] buffer full when inserting instruction %02X [%s]\n", 
                __func__, line->opcode->instr, line->opcode->mnemonic);
    }
    else if(status < 0)
    {
        fprintf(stdout, "[%s] failed to insert instruction %02X [%s]\n", 
                __func__, line->opcode->instr, line->opcode->mnemonic);
    }

    return status;
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
        status = assembler_assem_line(assem, cur_line);
        if(status < 0)
            break;
    }

    return status;
}
