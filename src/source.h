/*
 * SOURCE
 * Stuff for dealing with assembly source
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_SOURCE_H
#define __EMU_SOURCE_H

#define TOKEN_BUF_SIZE 16
#define LINE_INFO_NUM_REG 2

#include "opcode.h"

typedef enum
{
    REG_NONE,
    REG_A,
    REG_B,
    REG_C,
    REG_D,
    REG_E,
    REG_H,
    REG_L,
    REG_M,          // memory word
    REG_S,          // stack pointer
    REG_PSW         // program status word (A + flags)
} RegType;

extern const char* REG_TYPE_TO_STR[10];

// Text segment
typedef struct 
{
    Opcode* opcode;
    char*   label_str;
    char*   symbol_str;
    int     label_str_len;
    int     symbol_str_len;
    // Position
    int     line_num;
    int     addr;
    // arguments 
    int     has_immediate;
    int     immediate;
    RegType reg[LINE_INFO_NUM_REG];
    // error info
    int     error;
} LineInfo;

LineInfo* line_info_create(void);
void      line_info_destroy(LineInfo* info);
void      line_info_init(LineInfo* info);
void      line_info_print(LineInfo* info);
void      line_info_print_instr(LineInfo* info);
int       line_info_copy(LineInfo* dst, LineInfo* src);
int       line_info_struct_size(LineInfo* info);
int       line_info_set_label_str(LineInfo* info, char* label_str, int len);
int       line_info_set_symbol_str(LineInfo* info, char* symbol_str, int len);

/*
 * reg_char_to_code()
 * Convert a character to a register enum code
 */
uint8_t   reg_char_to_code(char r);


// ==== Buffer for LineInfo Structures
typedef struct
{
    LineInfo** buffer;
    int size;
    int max_size;
    int cur_line;       // TODO : why do we need to keep this var?
} SourceInfo;

SourceInfo* source_info_create(int num_lines);
void        source_info_destroy(SourceInfo* info);
int         source_info_add_line(SourceInfo* info, LineInfo* line);
int         source_info_edit_line(SourceInfo* info, LineInfo* line, int idx);
LineInfo*   source_info_get_idx(SourceInfo* info, int idx);
SourceInfo* source_info_clone(SourceInfo* src);
int         source_info_full(SourceInfo* info);
int         source_info_empty(SourceInfo* info);
int         source_info_write(SourceInfo* info, const char* filename);


// Data segment
typedef struct
{
    uint8_t* data;
    int      data_size;
    int      addr;
} DataSegment;

DataSegment* data_segment_create(int size);
void data_segment_destroy(DataSegment* segment);

// ======== TOKEN ======== //
typedef enum {
    SYM_NONE, 
    SYM_LITERAL, 
    SYM_LABEL, 
    SYM_INSTR,
    SYM_REG,
    SYM_STRING,
    SYM_EOF
} TokenType;

extern const char* TOKEN_TYPE_TO_STR[6];

/*
 * Token object
 */
typedef struct 
{
    TokenType type;
    int       token_str_len;
    char      token_str[TOKEN_BUF_SIZE];
} Token;

Token* create_token(void);
void   token_init(Token* token);
void   destroy_token(Token* token);



#endif /*__EMU_SOURCE_H*/
