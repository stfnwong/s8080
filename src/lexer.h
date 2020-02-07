/*
 * LEXER
 * Lexer for 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_LEXER_H
#define __EMU_LEXER_H

// This was reduced for debugging, but we can expand it back to 128 later
#define TOKEN_BUF_SIZE 32 

#include <stdint.h>
#include "opcode.h"
#include "source.h"


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
    char token_buf[TOKEN_BUF_SIZE];
    int  token_buf_ptr;

    // address bookkeeping
    int text_addr;
    int data_addr;

    // current line information
    LineInfo*    text_seg;      // TODO : where to put the collection of LineInfos?
    SourceInfo*  source_repr;
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
int    lex_parse_imm(Lexer* lexer, Token* tok);

// Lex a line in the source 
void   lex_line(Lexer* lexer);

int    lex_all(Lexer* lexer);


#endif /*__EMU_LEXER_H*/
