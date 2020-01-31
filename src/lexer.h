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
    char args[3];
    int line_num;
    int addr;
} LineInfo;

LineInfo* line_info_create(void);
void      line_info_destroy(LineInfo* info);
void      line_info_init(LineInfo* info);


// Data segment
typedef struct
{
    uint8_t* data;
    int      data_size;
    int      addr;
} DataSegment;

DataSegment* data_segment_create(int size);
void data_segment_destroy(DataSegment* segment);


// ======= File handling functions 
int lex_get_file_size(const char* filename);
int lex_read_file(const char* filename, char* buf, int buf_size);


// ======== Parsing ========= //

typedef enum {
    SYM_NONE, 
    SYM_LITERAL, 
    SYM_LABEL, 
    SYM_INSTR,
    SYM_EOF
} TokenType;


/*
 * Token object
 */
typedef struct 
{
    TokenType type;
    char      token_str[LEXER_TOKEN_BUF_SIZE];
} Token;

Token* create_token(void);
void   init_token(Token* token);
void   destroy_token(Token* token);

/*
 * Lexer object
 */
typedef struct
{
    // position info
    int  cur_pos;
    int  cur_line;
    int  cur_col;
    char cur_char;

    // Token buffers
    char token_buf[LEXER_TOKEN_BUF_SIZE];
    int  token_buf_ptr;

    // address bookkeeping
    int text_addr;
    int data_addr;

    // current line information
    LineInfo*    text_seg;
    //DataSegment* data_seg;
    
    // misc settings
    int verbose;

} Lexer;


Lexer* create_lexer(void);
void   destroy_lexer(Lexer* lexer);


// Move through source
int  lex_is_whitespace(const char c);
int  lex_is_comment(const char c);
void lex_advance(Lexer* lexer, const char* src, size_t src_size);
void lex_skip_whitespace(Lexer* lexer, const char* src, size_t src_size);
void lex_skip_comment(Lexer* lexer, const char* src, size_t src_size);

//  extract tokens
void lex_scan_token(Lexer* lexer, const char* src, size_t src_size);
void lex_next_token(Lexer* lexer, Token* token, const char* src, size_t src_size);

// Lex a line in the source 
void lex_line(Lexer* lexer, const char* src, size_t src_size);

int lex_file(Lexer* lexer, const char* src, size_t src_size);


#endif /*__EMU_LEXER_H*/
