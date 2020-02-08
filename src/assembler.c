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
 * instr_init()
 */
void instr_init(Instr* instr)
{
    instr->instr = 0;
    instr->addr = 0;
}

void instr_print(Instr* instr)
{
    fprintf(stdout, "[0x%04X] %02X", instr->addr, instr->instr);
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

// TODO: we can compress this down....

int asm_add(Instr* instr, LineInfo* line)
{
    instr->instr = 0x2 << 6;        
    instr->instr = instr->instr | (0x0 << 3);       // ADD code
    instr->instr = instr->instr | asm_reg_to_code(line->reg[0]);
    instr->addr  = line->addr;
}

int asm_adc(Instr* instr, LineInfo* line)
{
    instr->instr = 0x2 << 6;        
    instr->instr = instr->instr | (0x1 << 3);       // ADC code
    instr->instr = instr->instr | asm_reg_to_code(line->reg[0]);
    instr->addr  = line->addr;
}

int asm_inr(Instr* instr, LineInfo* line)
{
    instr->instr = 0x0 << 6;        
    instr->instr = instr->instr | asm_reg_to_code(line->reg[0]);
    instr->instr = instr->instr | 0x4;
    instr->addr  = line->addr;
}

int asm_xra(Instr* instr, LineInfo* line)
{
    instr->instr = 0x2 << 6;        
    instr->instr = instr->instr | (0x5 << 3);       // XRA code
    instr->instr = instr->instr | asm_reg_to_code(line->reg[0]);
    instr->addr  = line->addr;
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
    assem->instr_buf_size = 0;
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
    Instr** buffer;
   
    buffer = malloc(sizeof(Instr*) * size);
    if(!buffer)
        return NULL;

    for(int i = 0; i < size; ++i)
    {
        buffer[i] = malloc(sizeof(*buffer[i]));
        if(!buffer[i])
        {
            free(buffer);
            return NULL;
        }
    }

    return buffer;
}

/*
 * assembler_destroy_buffer()
 */
void assembler_destroy_buffer(Assembler* assem)
{
    for(int i = 0; i < assem->instr_buf_size; ++i)
        free(assem->instr_buf[i]);
    free(assem->instr_buf);
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

    assem->instr_buf = assembler_create_buffer(repr->size);
    if(assem->instr_buf == NULL)
    {
        fprintf(stdout, "[%s] failed to create instruction buffer for assembler object\n", __func__);
        return -1;
    }
    assem->instr_buf_size = repr->size;

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
                asm_adc(&cur_instr, line);
                break;

            case LEX_ADD:
                fprintf(stdout, "[%s] got LEX_ADD\n", __func__);
                asm_add(&cur_instr, line);
                break;

            case LEX_INR:
                asm_inr(&cur_instr, line);
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
