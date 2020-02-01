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

/*
 * opcode_init()
 * Set an Opcode back to 'initial' state.
 */
void opcode_init(Opcode* opcode)
{
    opcode->instr = 0;
    memset(opcode->mnemonic, 0, OPCODE_MNEMONIC_SIZE);
}

void opcode_print(Opcode* opcode)
{
    fprintf(stdout, "[%04X] : %s", opcode->instr, opcode->mnemonic);
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
