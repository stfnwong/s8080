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

Opcode* opcode_create(void);
void    opcode_destroy(Opcode* opcode);
void    opcode_init(Opcode* opcode);
void    opcode_print(Opcode* opcode);
void    opcode_copy(Opcode* dst, Opcode* src);


// Instruction codes used by Lexer
typedef enum {
    LEX_INVALID,
    LEX_ACI,
    LEX_ADC,
    LEX_ADD,
    LEX_ADI,
    LEX_ANA,
    LEX_AND,
    LEX_CMA,
    LEX_CMP,
    LEX_CZ,
    LEX_DAA,
    LEX_DAD,
    LEX_DCR,
    LEX_DCX,
    LEX_IN,
    LEX_INR,
    LEX_INX,
    LEX_JNZ,
    LEX_JP,
    LEX_JZ,
    LEX_LDA,
    LEX_LDAX,
    LEX_LHLD,
    LEX_LXI,
    LEX_MOV,
    LEX_MVI,
    LEX_NOP,
    LEX_ORA,
    LEX_ORI,
    LEX_OUT,
    LEX_POP,
    LEX_PUSH,
    LEX_RAL,
    LEX_SBB,
    LEX_STA,
    LEX_STAX,
    LEX_SUB,
    LEX_XRA,
    NUM_LEX_INSTR
} instr_code;

// Move to *.c file in next commit
extern const char* INSTR_CODE_TO_STR[7];
extern const Opcode LEX_INSRUCTIONS[5];

//static const int NUM_LEX_INSTR = 27;      // For now this needs to be manually updated


// Opcode lookup
typedef struct
{
    // Null opcode (to return when there are no matches)
    Opcode* null_op;
    Opcode* op_array;
    int     num_opcodes;
} OpcodeTable;


OpcodeTable* opcode_table_create(void);
void         opcode_table_destroy(OpcodeTable* optable);
void         opcode_print_table(OpcodeTable* optable);

// Get opcodes
void opcode_table_find_instr(OpcodeTable* optable, Opcode* op, uint8_t instr);
void opcode_table_find_mnemonic(OpcodeTable* optable, Opcode* op, const char* mnemonic);


#endif /*__EMU_OPCODE_H*/
