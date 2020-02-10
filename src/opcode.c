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

//const char* INSTR_CODE_TO_STR[] = {
//    "INVALID",
//    "ACI",
//    "ADC",
//    "ADD",
//    "ADI",
//    "ANA",
//    "AND",
//    "CMA",
//    "CMP",
//    "CZ",
//    "DAA",
//    "DAD",
//    "DCR",
//    "DCX",
//    "IN",
//    "INR",
//    "INX",
//    "JNZ",
//    "JP",
//    "JZ",
//    "LDA",
//    "LDAX",
//    "LHLD",
//    "LXI",
//    "MOV",
//    "MVI",
//    "NOP",
//    "ORA",
//    "ORI",
//    "POP",
//    "PUSH",
//    "RAL",
//    "SBB",
//    "STA",
//    "SUB",
//    "XRA"
//};

// All supported opcodes as Opcode structures
const Opcode LEX_INSTRUCTIONS[] = {
    {LEX_ACI,  "ACI"},
    {LEX_ADC,  "ADC"},
    {LEX_ADD,  "ADD"},
    {LEX_ADI,  "ADI"},
    {LEX_ANA,  "ANA"},
    {LEX_AND,  "AND"},
    {LEX_CMA,  "CMA"},
    {LEX_CMP,  "CMP"},
    {LEX_CZ,   "CZ"},
    {LEX_DAA,  "DAA"},
    {LEX_DAD,  "DAD"},
    {LEX_DCR,  "DCR"},
    {LEX_DCX,  "DCX"},
    {LEX_IN,   "IN"},
    {LEX_INR,  "INR"},
    {LEX_INX,  "INX"},
    {LEX_JNZ,  "JNZ"},
    {LEX_JP,   "JP"},
    {LEX_JZ,   "JZ"},
    {LEX_LDA,  "LDA"},
    {LEX_LDAX, "LDAX"},
    {LEX_LHLD, "LHLD"},
    {LEX_LXI,  "LXI"},
    {LEX_MOV,  "MOV"},
    {LEX_MVI,  "MVI"},
    {LEX_NOP,  "NOP"},
    {LEX_ORA,  "ORA"},
    {LEX_ORI,  "ORI"},
    {LEX_POP,  "POP"},
    {LEX_PUSH, "PUSH"},
    {LEX_RAL,  "RAL"},
    {LEX_SBB,  "SBB"},
    {LEX_STA,  "STA"},
    {LEX_STAX, "STAX"},
    {LEX_SUB,  "SUB"},
    {LEX_XRA,  "XRA"},
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
        fprintf(stderr, "[%s] Failed to allocate memory for opcode\n", __func__);
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
    if(opcode != NULL)
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
    int status = 0;
    OpcodeTable* table;

    table = malloc(sizeof(*table));
    if(!table)
    {
        fprintf(stderr, "[%s] failed to allocate memory for OpcodeTable\n", __func__);
        status = -1;
        goto OPCODE_TABLE_END;
    }
    table->null_op = NULL;
    table->op_array = NULL;

    table->null_op = malloc(sizeof(Opcode));
    if(!table->null_op)
    {
        fprintf(stderr, "[%s] failed to allocate memory for Null Opcode\n", __func__);
        status = -1;
        goto OPCODE_TABLE_END;
    }
    opcode_init(table->null_op);

    // Creare an array of opcodes to search
    table->num_opcodes = NUM_LEX_INSTR;
    table->op_array = malloc(sizeof(*table->op_array) * table->num_opcodes);
    if(!table->op_array)
    {
        fprintf(stderr, "[%s] failed to allocate memory for op_array\n", __func__);
        status = -1;
        goto OPCODE_TABLE_END;
    }
    // Try allocate table memory
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        fprintf(stdout, "[%s] allocating opcode [%d / %d]\n",
                __func__, i+1, table->num_opcodes);
        table->op_array[i] = malloc(sizeof(*table->op_array[i]));
        if(!table->op_array[i])
        {
            status = -1;
            goto OPCODE_TABLE_END;
        }
        table->op_array[i] = opcode_create();
        if(!table->op_array[i])
        {
            fprintf(stderr, "[%s] failed to allocate memory for opcode %d\n", __func__, i);
            status = -1;
            goto OPCODE_TABLE_END;
        }
    }

    // Add instructions to to the table
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        opcode_copy(table->op_array[i], &LEX_INSTRUCTIONS[i]);
        //table->op_array[i]->instr    = LEX_INSTRUCTIONS[i].instr;
        //strncpy(table->op_array[i]->mnemonic, 
        //        LEX_INSTRUCTIONS[i].mnemonic, 
        //        strlen(LEX_INSTRUCTIONS[i].mnemonic)
        //);
        fprintf(stdout, "[%s] copied instruction %d / %d with mnemonic %s of len %ld\n", __func__, i+1, table->num_opcodes, LEX_INSTRUCTIONS[i].mnemonic, strlen(LEX_INSTRUCTIONS[i].mnemonic));
        //table->op_array[i]->mnemonic = LEX_INSTRUCTIONS[i].mnemonic;
    }

OPCODE_TABLE_END:
    if(status < 0)
    {
        fprintf(stderr, "[%s] failed to create OpcodeTable\n", __func__);
        if(table->op_array)
            free(table->op_array);
        free(table);
        return NULL;
    }

    return table;
}

/*
 * opcode_table_destroy()
 */
void opcode_table_destroy(OpcodeTable* optable)
{
    if(optable == NULL)
        free(optable);
    else
    {
        for(int i = 0; i < optable->num_opcodes; ++i)
        {
            fprintf(stdout, "[%s] freeing opcode [%d / %d]\n",
                    __func__, i+1, optable->num_opcodes);
            free(optable->op_array[i]);
        }
        //free(optable->null_op);
        free(optable->op_array);
        free(optable);
    }
}

/*
 * opcode_table_print()
 */
void opcode_table_print(OpcodeTable* optable)
{
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        fprintf(stdout, "[%2d] : ", i+1);
        opcode_print(optable->op_array[i]);
        fprintf(stdout, "\n");
    }
}

/*
 * opcode_table_find_instr()
 */
void opcode_table_find_instr(OpcodeTable* optable, Opcode* op, uint8_t instr)
{
    fprintf(stdout, "[%s] searching table...\n", __func__);
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        fprintf(stdout, "[%s] checking op %d/%d\n", __func__, i+1, optable->num_opcodes);
        if(instr == optable->op_array[i]->instr)
        {
            fprintf(stdout, "[%s] instr %0X == optable->array[%d] : %0X [%s]\n", __func__, instr, i, optable->op_array[i]->instr, optable->op_array[i]->mnemonic);
            op->instr = optable->op_array[i]->instr;
            strncpy(op->mnemonic, optable->op_array[i]->mnemonic, OPCODE_MNEMONIC_SIZE);
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
        if(strcmp(mnemonic, optable->op_array[i]->mnemonic) == 0)
        {
            op->instr = optable->op_array[i]->instr;
            strncpy(op->mnemonic, optable->op_array[i]->mnemonic, OPCODE_MNEMONIC_SIZE);
            return;
        }
    }

    op->instr = optable->null_op->instr;
    strncpy(op->mnemonic, optable->null_op->mnemonic, OPCODE_MNEMONIC_SIZE);
}
