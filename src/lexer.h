/*
 * LEXER
 * Lexer for 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_LEXER_H
#define __EMU_LEXER_H

// This was reduced for debugging, but we can expand it back to 128 later
#define LEXER_TOKEN_BUF_SIZE 32 

#include <stdint.h>
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
    char  reg[3];
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
    char      token_str[LEXER_TOKEN_BUF_SIZE];
} Token;

Token* create_token(void);
void   token_init(Token* token);
void   destroy_token(Token* token);

/*
 * Lexer object
 */
typedef struct
{
    // Source info 
    char* src;
    int   src_len;
    // position info
    int   cur_pos;
    int   cur_line;
    int   cur_col;
    char  cur_char;

    // Token buffers
    char token_buf[LEXER_TOKEN_BUF_SIZE];
    int  token_buf_ptr;

    // address bookkeeping
    int text_addr;
    int data_addr;

    // current line information
    LineInfo*    text_seg;      // TODO : where to put the collection of LineInfos?
    //DataSegment* data_seg;
    
    // Opcode table
    OpcodeTable* op_table;

    // misc settings
    int verbose;

} Lexer;

Lexer* lexer_create(void);
void   lexer_destroy(Lexer* lexer);
int    lex_read_file(Lexer* lexer, const char* filename);

// Move through source
int    lex_is_whitespace(const char c);
int    lex_is_comment(const char c);
void   lex_advance(Lexer* lexer);
void   lex_skip_whitespace(Lexer* lexer);
void   lex_skip_comment(Lexer* lexer);

// Update addreses
void   lex_text_addr_incr(Lexer* lexer);
void   lex_data_addr_incr(Lexer* lexer);

//  extract tokens
void   lex_scan_token(Lexer* lexer);
void   lex_next_token(Lexer* lexer, Token* token);    // TODO: cur token?

// Parse instructions 
int    lex_parse_one_reg(Lexer* lexer, Token* token);
int    lex_parse_two_reg(Lexer* lexer, Token* tok_a, Token* tok_b);
int    lex_parse_reg_imm(Lexer* lexer, Token* tok_a, Token* tok_b);

// Lex a line in the source 
void   lex_line(Lexer* lexer);

int    lex_all(Lexer* lexer);


#endif /*__EMU_LEXER_H*/
