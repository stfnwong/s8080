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

// Text segment
typedef struct 
{
    Opcode* opcode;
    // Position
    int   line_num;
    int   addr;
    
    // arguments 
    int   has_immediate;
    int   immediate;
    char  reg[LINE_INFO_NUM_REG];
    char* label_str;
    int   label_str_len;
    // error info
    int   error;
} LineInfo;

LineInfo* line_info_create(void);
void      line_info_destroy(LineInfo* info);
void      line_info_init(LineInfo* info);
void      line_info_print(LineInfo* info);
int       line_info_copy(LineInfo* dst, LineInfo* src);

// ==== Buffer for LineInfo Structures
typedef struct
{
    LineInfo** buffer;
    int size;
    int max_size;
    int cur_line;
} SourceInfo;

SourceInfo* source_info_create(int num_lines);
void        source_info_destroy(SourceInfo* info);
int         source_info_add_line(SourceInfo* info, LineInfo* line);
int         source_info_edit_line(SourceInfo* info, LineInfo* line, int idx);
LineInfo*   source_info_get_idx(SourceInfo* info, int idx);
SourceInfo* source_info_clone(SourceInfo* src);
int         source_info_full(SourceInfo* info);
int         source_info_empty(SourceInfo* info);

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
    SYM_EOF
} TokenType;

extern const char* TOKEN_TYPE_TO_STR[6];

/*
 * Token object
 */
typedef struct 
{
    TokenType type;
    char      token_str[TOKEN_BUF_SIZE];
} Token;

Token* create_token(void);
void   token_init(Token* token);
void   destroy_token(Token* token);



#endif /*__EMU_SOURCE_H*/
