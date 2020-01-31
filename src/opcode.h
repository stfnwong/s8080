/*
 * OPCODE
 * 8080 Opcodes and instructions 
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_OPCODE_H
#define __EMU_OPCODE_H

#define OPCODE_MNEMONIC_SIZE 8

#include <stdint.h>

// An opcode
typedef struct 
{
    uint8_t instr;
    char    mnemonic[OPCODE_MNEMONIC_SIZE];
} Opcode;

void init_opcode(Opcode* opcode);
void print_opcode(Opcode* opcode);


// Instruction codes used by Lexer
typedef enum {
    LEX_INVALID,
    LEX_DCR,
    LEX_INR,
    LEX_MOV,
    LEX_MVI,
    LEX_NOP
} instr_code;

const static Opcode LEX_INSTRUCTIONS[] = {
    {LEX_DCR, "DCR"},
    {LEX_INR, "INR"},
    {LEX_MOV, "MOV"},
    {LEX_MVI, "MVI"},
    {LEX_NOP, "NOP"}
};
static const int NUM_LEX_INSTR = 5;      // For now this needs to be manually updated


// Opcode lookup
typedef struct
{
    // Null opcode (to return when there are no matches)
    Opcode* null_op;
    Opcode* op_array;
    int     num_opcodes;
} OpcodeTable;


OpcodeTable* opcode_table_create(void);
void opcode_table_destroy(OpcodeTable* optable);
void print_opcode_table(OpcodeTable* optable);

// Get opcodes
void opcode_table_find_instr(OpcodeTable* optable, Opcode* op, uint8_t instr);
//void opcode_table_find_mnemonic(OpcodeTable* optable, Opcode* op, const char* mnemonic);


#endif /*__EMU_OPCODE_H*/
