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


// ROUTINES FOR EACH INSTRUCTION
// TODO : Data segment

// This array works for
// - register or memory to accumulator instructions
// - single register instructions
const uint8_t asm_reg_to_code[] = {
    0x0,    // REG_NONE
    0x7,    // REG_A
    0x0,    // REG_B
    0x1,    // REG_C
    0x2,    // REG_D
    0x3,    // REG_E,
    0x4,    // REG_H
    0x5,    // REG_L,
    0x6,    // REG_M
};

/*
 * asm_pair_reg_to_code()
 */
uint8_t asm_pair_reg_to_code(uint8_t reg)
{
    switch(reg)
    {
        case REG_B:
        case REG_C:
            return 0x0;
        case REG_D:
        case REG_E:
            return 0x1;
        case REG_H:
        case REG_L:
            return 0x2;
        case REG_S:
            return 0x3;
    }

    return 0x0;
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
    dst->instr = dst->instr | asm_reg_to_code[line->reg[0]];
    dst->addr  = line->addr;
}

/*
 * asm_16bit_arith()
 */
void asm_16bit_arith(Instr* dst, LineInfo* line)
{
}

/*
 * asm_reg_pair()
 */
void asm_reg_pair(Instr* dst, LineInfo* line, uint8_t op)
{
    dst->instr = op;
    dst->instr = asm_pair_reg_to_code(line->reg[0]) << 4;
    dst->addr  = dst->addr;
}

/*
 * asm_imm_2byte()
 */
void asm_imm_2byte(Instr* dst, LineInfo* line, uint8_t op)
{
    dst->instr = (0x3 << 14) | (op << 11) | (0x6 << 8);
    dst->instr = dst->instr | line->immediate;
    dst->size = 2;
    dst->addr = line->addr;
}

/*
 * asm_imm_3byte()
 */
void asm_imm_3byte(Instr* dst, LineInfo* line)
{
    dst->instr = 0x1 << 9;
    dst->size = 3;
    dst->addr = line->addr;
}

void asm_lxi(Instr* dst, LineInfo* line)
{

}

/*
 * asm_mvi()
 */
void asm_mvi(Instr* dst, LineInfo* line)
{
    dst->instr = (0x6 << 8);
    dst->instr = dst->instr | (asm_reg_to_code[line->reg[0]] << 11);
    dst->instr = dst->instr | line->immediate;
    dst->size  = 2;
    dst->addr  = line->addr;
}

/*
 * asm_ldax()
 */
void asm_ldax(Instr* dst, LineInfo* line)
{
    dst->instr = 0x9;
    if(line->reg[0] == REG_D)
        dst->instr = dst->instr | (0x1 << 4);
    dst->addr = line->addr;
}

/*
 * asm_stax()
 */
void asm_stax(Instr* dst, LineInfo* line)
{
    dst->instr = 0x1 << 1;
    if(line->reg[0] == REG_D)
        dst->instr = dst->instr | (0x1 << 4);
    dst->addr = line->addr;
}

/*
 * asm_mov
 */
void asm_mov(Instr* dst, LineInfo* line)
{
    dst->instr = asm_mov_instr_to_code(line->opcode->instr);
    dst->instr = dst->instr | (asm_reg_to_code[line->reg[0]] << 3);
    dst->instr = dst->instr |  asm_reg_to_code[line->reg[1]];
    dst->addr = line->addr;
}

/*
 * asm_jp_to_code()
 * 3 bytes
 */
uint32_t asm_jp_to_code(uint8_t instr)
{
    uint32_t code = (0x3 << 22) | (0x1 << 18);

    switch(instr)
    {
        case LEX_JMP:
        case LEX_JNZ:
            code = code | (0x1 << 17);
            break;
        case LEX_JZ:
            code = code | (0x1 << 19);
            break;
        case LEX_JNC:
            code = code | (0x2 << 19);
            break;
        case LEX_JC:
            code = code | (0x3 << 19);
            break;
        case LEX_JPO:
            code = code | (0x4 << 19);
            break;
        case LEX_JPE:
            code = code | (0x5 << 19);
            break;
        case LEX_JP:
            code = code | (0x6 << 19);
            break;
        case LEX_JM:
            code = code | (0x7 << 19);
            break;
    }

    return code;
}

/*
 * asm_jp()
 */
void asm_jp(Instr* dst, LineInfo* line)
{
    dst->instr = asm_jp_to_code(line->opcode->instr);
    dst->instr = dst->instr | ((line->immediate & 0x00FF) << 8);
    dst->instr = dst->instr | (line->immediate & 0xFF00);
    dst->size  = 3;
    dst->addr  = line->addr;
}

/*
 * asm_ret_to_code()
 */
uint8_t asm_ret_to_code(uint8_t instr)
{
    uint8_t code = 0x3 << 6;
    
    switch(instr)
    {
        case LEX_RET:
            code = code | 0x9;
            break;
        case LEX_RC:
            code = code | (0x3 << 3);
            break;
        case LEX_RNC:
            code = code | (0x2 << 3);
            break;
        case LEX_RZ:
            code = code | (0x1 << 3);
            break;
        case LEX_RNZ:
            break;
        case LEX_RM:
            code = code | (0x7 << 3);
            break;
        case LEX_RP:
            code = code | (0x6 << 3);
            break;
        case LEX_RPE:
            code = code | (0x5 << 3);
            break;
        case LEX_RPO:
            code = code | (0x4 << 3);
            break;
    }

    return code;
}

/*
 * asm_ret()
 */
void asm_ret(Instr* dst, LineInfo* line)
{
    dst->instr = asm_ret_to_code(line->opcode->instr);
    dst->size  = 1;
    dst->addr  = line->addr;
}

/*
 * asm_call_to_code()
 */
uint16_t asm_call_to_code(uint8_t instr)
{
    uint16_t code = (0x3 << 14) | (0x1 << 10);

    switch(instr)
    {
        case LEX_CALL:
            code = code | (0x1 << 11) | (0x1 << 9);
            break;
        case LEX_CZ:
            code = code | (0x1 << 11);
            break;
        case LEX_CC:
            code = code | (0x3 << 11);
            break;
        case LEX_CPO:
            code = code | (0x4 << 11);
            break;
        case LEX_CPE:
            code = code | (0x5 << 11);
            break;
        case LEX_CP:
            code = code | (0x6 << 11);
            break;
        case LEX_CM:
            code = code | (0x7 << 11);
            break;
    }

    return code;
}

/*
 * asm_call()
 */
void asm_call(Instr* dst, LineInfo* line)
{
    dst->instr = asm_call_to_code(line->opcode->instr);
    dst->instr = dst->instr | ((line->immediate & 0x00FF) << 8);
    dst->instr = dst->instr | (line->immediate & 0xFF00);
    dst->size = 2;
    dst->addr = line->addr;
}

/*
 * asm_data()
 */

void asm_data(InstrVector* vec, LineInfo* line)
{
    int cur_addr;

    ByteNode* cur_node;
    Instr cur_instr;

    cur_addr = line->addr;
    for(int i = 0; i < byte_list_len(line->byte_list); ++i)
    {
        cur_node = byte_list_get(line->byte_list, i);
        while(cur_node != NULL)
        {
            for(int d = 0; d < cur_node->len; ++d)
            {
                cur_instr.instr = cur_node->data[d];
                cur_instr.addr  = cur_addr;
                cur_instr.size  = 1;
                instr_vector_push_back(vec, &cur_instr);
                cur_addr++;
            }
            cur_node = cur_node->next;
        }
    }
}

// ================ ASSEMBLER OBJECT ================ //

/*
 * assembler_create()
 */
Assembler* assembler_create(void)
{
    Assembler* assem;

    assem = malloc(sizeof(*assem));
    if(!assem)
        goto ASSEM_CREATE_END;

    assem->src_repr = NULL;
    assem->cur_line = 0;
    assem->verbose  = 0;

    assem->instr_buf = instr_vector_create(8);
    if(!assem->instr_buf)
        goto ASSEM_CREATE_END;

ASSEM_CREATE_END:
    if(!assem || !assem->instr_buf)
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
    instr_vector_destroy(assem->instr_buf);
    free(assem);
}

/*
 * assembler_set_repr()
 */
void assembler_set_repr(Assembler* assem, SourceInfo* repr)
{
    assem->src_repr = repr;
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
            case LEX_POP:
            case LEX_PUSH:
            case LEX_SPHL:
            case LEX_XCHG:
                asm_mov(&cur_instr, line);
                break;

            // jump instructions
            case LEX_JP:
            case LEX_JMP:
            case LEX_JC:
            case LEX_JNC:
            case LEX_JZ:
            case LEX_JNZ:
                asm_jp(&cur_instr, line);
                break;

            // subroutine call instructions 
            case LEX_CALL:
            case LEX_CC:
                break;

            // subroutine return instructions 
            case LEX_RET:
            case LEX_RC:
            case LEX_RNC:
            case LEX_RZ:
            case LEX_RNZ:
            case LEX_RM:
            case LEX_RP:
            case LEX_RPE:
            case LEX_RPO:
                asm_ret(&cur_instr, line);
                break;

            // data instructions 
            case LEX_DB:
            case LEX_DS:
            case LEX_DW:
                asm_data(assem->instr_buf, line);
                status = 0;     // is there anything that can go wrong?
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

    // TODO : debug, remove
    fprintf(stdout, "[%s] about to insert instruction :", __func__);
    instr_print(&cur_instr);
    fprintf(stdout, "\n");

    instr_vector_push_back(assem->instr_buf, &cur_instr);
    //if(status < 0)
    //{
    //    fprintf(stdout, "[%s] failed to insert instruction %02X [%s]\n", 
    //            __func__, line->opcode->instr, line->opcode->mnemonic);
    //}

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
    
    LineInfo* cur_line;
    for(int l = 0; l < assem->src_repr->size; ++l)
    {
        cur_line = assem->src_repr->buffer[l];
        status = assembler_assem_line(assem, cur_line);
        if(status < 0)
            break;
    }

    return status;
}


/*
 * assembler_get_instr_vector()
 */
InstrVector* assembler_get_instr_vector(Assembler* assem)
{
    return assem->instr_buf;
}


// -------- MISC -------- //
void assembler_set_verbose(Assembler* assem)
{
    assem->verbose = 1;
}

void assembler_clear_verbose(Assembler* assem)
{
    assem->verbose = 0;
}

int assembler_verbose(Assembler* assem)
{
    return assem->verbose;
}
