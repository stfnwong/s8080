/*
 * OPCODE
 * 8080 Opcodes and instructions 
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcode.h"

const char* INSTR_CODE_TO_STR[] = {
    "INVALID",
    "ADC",
    "ADD",
    "ADI",
    "ANA",
    "AND",
    "CMP",
    "DAD",
    "DCR",
    "INR",
    "INX",
    "JNZ",
    "JP",
    "JZ",
    "LDA",
    "LDAX",
    "MOV",
    "MVI",
    "NOP",
    "ORA",
    "ORI",
    "POP",
    "PUSH",
    "SBB",
    "SUB",
    "XRA"
};

const Opcode LEX_INSTRUCTIONS[] = {
    {LEX_ADC, "ADC"},
    {LEX_ADD, "ADD"},
    {LEX_ADI, "ADI"},
    {LEX_ANA, "ANA"},
    {LEX_AND, "AND"},
    {LEX_CMP, "CMP"},
    {LEX_DAD, "DAD"},
    {LEX_DCR, "DCR"},
    {LEX_INR, "INR"},
    {LEX_INX, "INX"},
    {LEX_JNZ, "JNZ"},
    {LEX_JP, "JP"},
    {LEX_JZ, "JZ"},
    {LEX_LDA, "LDA"},
    {LEX_LDAX, "LDAX"},
    {LEX_MOV, "MOV"},
    {LEX_MVI, "MVI"},
    {LEX_NOP, "NOP"},
    {LEX_ORA, "ORA"},
    {LEX_ORI, "ORI"},
    {LEX_POP, "POP"},
    {LEX_PUSH, "PUSH"},
    {LEX_SBB, "SBB"},
    {LEX_STAX, "STAX"},
    {LEX_SUB, "SUB"},
    {LEX_XRA, "XRA"},
};

/*
 * opcode_create()
 */
Opcode* opcode_create(void)
{
    Opcode* opcode;

    opcode = malloc(sizeof(*opcode));
    if(!opcode)
    {
        fprintf(stderr, "[%s] Failed to allocate memory for opocde\n", __func__);
        return NULL;
    }

    return opcode;
}

/*
 * opcode_destroy()
 */
void opcode_destroy(Opcode* opcode)
{
    free(opcode);
}

/*
 * opcode_init()
 * Set an Opcode back to 'initial' state.
 */
void opcode_init(Opcode* opcode)
{
    opcode->instr = 0;
    memset(opcode->mnemonic, 0, OPCODE_MNEMONIC_SIZE);
}

/*
 * opcode_print()
 */
void opcode_print(Opcode* opcode)
{
    fprintf(stdout, "%s (%04X)", opcode->mnemonic, opcode->instr);
}

/*
 * opcode_copy()
 */
void opcode_copy(Opcode* dst, Opcode* src)
{
    dst->instr = src->instr;
    memcpy(dst->mnemonic, src->mnemonic, OPCODE_MNEMONIC_SIZE);
}

// ======== OPCODE TABLE ======== //
OpcodeTable* opcode_table_create(void)
{
    OpcodeTable* table;

    table = malloc(sizeof(*table));
    if(!table)
    {
        fprintf(stderr, "[%s] failed to allocate memory for OpcodeTable\n", __func__);
        return NULL;
    }

    table->null_op = malloc(sizeof(Opcode));
    if(!table->null_op)
    {
        fprintf(stderr, "[%s] failed to allocate memory for Null Opcode\n", __func__);
        return NULL;
    }
    opcode_init(table->null_op);

    // Creare an array of opcodes to search
    table->num_opcodes = NUM_LEX_INSTR;
    table->op_array = malloc(sizeof(Opcode) * table->num_opcodes);
    if(!table->op_array)
    {
        fprintf(stderr, "[%s] failed to allocate memory for op_array\n", __func__);
        return NULL;
    }

    // Add instructions to to the table
    for(int instr = 0; instr < table->num_opcodes; ++instr)
    {
        table->op_array[instr] = LEX_INSTRUCTIONS[instr];
    }

    return table;
}

/*
 * opcode_table_destroy()
 */
void opcode_table_destroy(OpcodeTable* optable)
{
    free(optable->null_op);
    free(optable->op_array);
    free(optable);
}

/*
 * opcode_print_table()
 */
void opcode_print_table(OpcodeTable* optable)
{
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        fprintf(stdout, "[%2d] : ", i+1);
        opcode_print(&optable->op_array[i]);
        fprintf(stdout, "\n");
    }
}

/*
 * opcode_table_find_instr()
 */
void opcode_table_find_instr(OpcodeTable* optable, Opcode* op, uint8_t instr)
{
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        if(instr == optable->op_array[i].instr)
        {
            op->instr = optable->op_array[i].instr;
            strncpy(op->mnemonic, optable->op_array[i].mnemonic, OPCODE_MNEMONIC_SIZE);
            return;
        }
    }

    op->instr = optable->null_op->instr;
    strncpy(op->mnemonic, optable->null_op->mnemonic, OPCODE_MNEMONIC_SIZE);
}

/*
 * opcode_table_find_mnemonic()
 */
void opcode_table_find_mnemonic(OpcodeTable* optable, Opcode* op, const char* mnemonic)
{
    // Just linearly compare strings. Maybe sort them or something later
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        if(strcmp(mnemonic, optable->op_array[i].mnemonic) == 0)
        {
            op->instr = optable->op_array[i].instr;
            strncpy(op->mnemonic, optable->op_array[i].mnemonic, OPCODE_MNEMONIC_SIZE);
            return;
        }
    }

    op->instr = optable->null_op->instr;
    strncpy(op->mnemonic, optable->null_op->mnemonic, OPCODE_MNEMONIC_SIZE);
}
