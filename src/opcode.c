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
 * init_opcode()
 * Set an Opcode back to 'initial' state.
 */
void init_opcode(Opcode* opcode)
{
    opcode->instr = 0;
    memset(opcode->mnemonic, 0, OPCODE_MNEMONIC_SIZE);
}

void print_opcode(Opcode* opcode)
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
    init_opcode(table->null_op);

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
        table->op_array[instr] = lex_instructions[instr];
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
 * print_opcode_table()
 */
void print_opcode_table(OpcodeTable* optable)
{
    for(int i = 0; i < optable->num_opcodes; ++i)
    {
        fprintf(stdout, "[%2d] : ", i+1);
        print_opcode(&optable->op_array[i]);
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
