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
void    opcode_copy(Opcode* dst, const Opcode* src);


// Instruction codes used by Lexer
typedef enum {
    LEX_INVALID,
    LEX_ACI,
    LEX_ADC,
    LEX_ADD,
    LEX_ADI,
    LEX_ANA,
    LEX_AND,
    LEX_ANI,
    LEX_CALL,
    LEX_CC,
    LEX_CM,
    LEX_CMA,
    LEX_CMP,
    LEX_CNZ,
    LEX_CP,
    LEX_CPE,
    LEX_CPI,
    LEX_CPO,
    LEX_CZ,
    LEX_DAA,
    LEX_DAD,
    LEX_DCR,
    LEX_DCX,
    LEX_DB,
    LEX_DS,
    LEX_DW,
    LEX_IN,
    LEX_INR,
    LEX_INX,
    LEX_JC,
    LEX_JM,
    LEX_JMP,
    LEX_JNC,
    LEX_JNZ,
    LEX_JP,
    LEX_JPE,
    LEX_JPO,
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
    LEX_PCHL,
    LEX_POP,
    LEX_PUSH,
    LEX_RAA,
    LEX_RAL,
    LEX_RAR,
    LEX_RC,
    LEX_RET,
    LEX_RLC,
    LEX_RM,
    LEX_RNC,
    LEX_RP,
    LEX_RPE,
    LEX_RPO,
    LEX_RRC,
    LEX_RZ,
    LEX_RNZ,
    LEX_SBB,
    LEX_SBI,
    LEX_SPHL,
    LEX_STA,
    LEX_STAX,
    LEX_SUB,
    LEX_SUI,
    LEX_XCHG,
    LEX_XTHL,
    LEX_XRA,
    LEX_XRI,
    //NUM_LEX_INSTR
} instr_code;

// Move to *.c file in next commit
static const int NUM_LEX_INSTR = 75;  // TODO :  better way of doing this
extern const Opcode LEX_INSTRUCTIONS[75];

// Psuedo ops / assembler directives
typedef enum 
{
    DIR_INVALID,
    DIR_END,
    DIR_ENDIF,
    DIR_ENDM,
    DIR_EQU,
    DIR_IF,
    DIR_MACRO,
    DIR_ORG,
    DIR_SET
} directive_code;

// Number of directives (for reference outside this file)
static const int NUM_LEX_DIR = 9;
extern const Opcode LEX_DIRECTIVES[9];

// Opcode lookup
typedef struct
{
    // Null opcode (to return when there are no matches)
    Opcode*  null_op;
    Opcode** op_array;
    int      num_opcodes;
} OpcodeTable;

OpcodeTable* opcode_table_create(void);
OpcodeTable* opcode_table_create_dir(void);
void         opcode_table_destroy(OpcodeTable* optable);
void         opcode_table_print(OpcodeTable* optable);

// Get opcodes
void opcode_table_find_instr(OpcodeTable* optable, Opcode* op, uint8_t instr);
void opcode_table_find_mnemonic(OpcodeTable* optable, Opcode* op, const char* mnemonic);


#endif /*__EMU_OPCODE_H*/
