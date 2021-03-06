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


// All supported opcodes as Opcode structures
const Opcode LEX_INSTRUCTIONS[] = {
    {LEX_INVALID, "INVALID"},
    {LEX_ACI,  "ACI"},
    {LEX_ADC,  "ADC"},
    {LEX_ADD,  "ADD"},
    {LEX_ADI,  "ADI"},
    {LEX_ANA,  "ANA"},
    {LEX_AND,  "AND"},
    {LEX_ANI,  "ANI"},
    {LEX_CALL, "CALL"},
    {LEX_CC,   "CC"},
    {LEX_CM,   "CM"},
    {LEX_CMA,  "CMA"},
    {LEX_CMP,  "CMP"},
    {LEX_CNZ,  "CNZ"},
    {LEX_CP,   "CP"},
    {LEX_CPE,  "CPE"},
    {LEX_CPI,  "CPI"},
    {LEX_CPO,  "CPO"},
    {LEX_CZ,   "CZ"},
    {LEX_DAA,  "DAA"},
    {LEX_DAD,  "DAD"},
    {LEX_DCR,  "DCR"},
    {LEX_DCX,  "DCX"},
    {LEX_DB,   "DB"},
    {LEX_DS,   "DS"},
    {LEX_DW,   "DW"},
    {LEX_IN,   "IN"},
    {LEX_INR,  "INR"},
    {LEX_INX,  "INX"},
    {LEX_JC,   "JC"},
    {LEX_JM,   "JM"},
    {LEX_JMP,  "JMP"},
    {LEX_JNC,  "JNC"},
    {LEX_JNZ,  "JNZ"},
    {LEX_JP,   "JP"},
    {LEX_JPE,  "JPE"},
    {LEX_JPO,  "JPO"},
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
    {LEX_PCHL, "PCHL"},
    {LEX_POP,  "POP"},
    {LEX_PUSH, "PUSH"},
    {LEX_RAA,  "RAA"},
    {LEX_RAL,  "RAL"},
    {LEX_RAR,  "RAR"},
    {LEX_RC,   "RC"},
    {LEX_RET,  "RET"},
    {LEX_RLC,  "RLC"},
    {LEX_RM,   "RM"},
    {LEX_RNC,  "RNC"},
    {LEX_RP,   "RP"},
    {LEX_RPE,  "RPE"},
    {LEX_RPO,  "RPO"},
    {LEX_RRC,  "RRC"},
    {LEX_RZ,   "RZ"},
    {LEX_RNZ,  "RNZ"},
    {LEX_SBB,  "SBB"},
    {LEX_SBI,  "SBB"},
    {LEX_SPHL, "SPHL"},
    {LEX_STA,  "STA"},
    {LEX_STAX, "STAX"},
    {LEX_SUB,  "SUB"},
    {LEX_SUI,  "SUI"},
    {LEX_XCHG, "XCHG"},
    {LEX_XTHL, "XTHL"},
    {LEX_XRA,  "XRA"},
    {LEX_XRI,  "XRI"},
};

// Directive codes
const Opcode LEX_DIRECTIVES[] = 
{
    {DIR_INVALID, "INVALID"},
    {DIR_END,     "END"},
    {DIR_ENDIF,   "ENDIF"},
    {DIR_ENDM,    "ENDM"},
    {DIR_EQU,     "EQU"},
    {DIR_IF,      "IF"},
    {DIR_MACRO,   "MACRO"},
    {DIR_ORG,     "ORG"},
    {DIR_SET,     "SET"}
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
void opcode_copy(Opcode* dst, const Opcode* src)
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
        status = -1;
        goto OPCODE_TABLE_END;
    }

    // Try allocate table memory
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        table->op_array[i] = malloc(sizeof(*table->op_array[i]));
        if(!table->op_array[i])
        {
            status = -1;
            goto OPCODE_TABLE_END;
        }
        table->op_array[i] = opcode_create();
        if(!table->op_array[i])
        {
            status = -1;
            goto OPCODE_TABLE_END;
        }
    }

    // Add instructions to to the table
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        opcode_copy(table->op_array[i], &LEX_INSTRUCTIONS[i]);
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
 * opcode_table_create_dir()
 */
OpcodeTable* opcode_table_create_dir(void)
{
    int status = 0;
    OpcodeTable* table;

    table = malloc(sizeof(*table));
    if(!table)
    {
        status = -1;
        goto OPCODE_TABLE_DIR_END;
    }
    table->null_op = NULL;
    table->op_array = NULL;

    table->null_op = malloc(sizeof(Opcode));
    if(!table->null_op)
    {
        fprintf(stderr, "[%s] failed to allocate memory for Null Opcode\n", __func__);
        status = -1;
        goto OPCODE_TABLE_DIR_END;
    }
    opcode_init(table->null_op);

    // Creare an array of opcodes to search
    table->num_opcodes = NUM_LEX_DIR;
    table->op_array = malloc(sizeof(*table->op_array) * table->num_opcodes);
    if(!table->op_array)
    {
        status = -1;
        goto OPCODE_TABLE_DIR_END;
    }

    // Try allocate table memory
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        table->op_array[i] = malloc(sizeof(*table->op_array[i]));
        if(!table->op_array[i])
        {
            status = -1;
            goto OPCODE_TABLE_DIR_END;
        }
        table->op_array[i] = opcode_create();
        if(!table->op_array[i])
        {
            status = -1;
            goto OPCODE_TABLE_DIR_END;
        }
    }

    // Add directives to to the table
    for(int i = 0; i < table->num_opcodes; ++i)
    {
        opcode_copy(table->op_array[i], &LEX_DIRECTIVES[i]);
    }

OPCODE_TABLE_DIR_END:
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
            free(optable->op_array[i]);
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
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        if(instr == optable->op_array[i]->instr)
        {
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
