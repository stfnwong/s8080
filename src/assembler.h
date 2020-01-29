/*
 * ASSEMBLER
 * 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_ASSEM_H
#define __EMU_ASSEM_H

#define LEXER_TOKEN_BUF_SIZE 128

#include <stdint.h>

// ======= File handling functions 
int asm_get_file_size(const char* filename);
int asm_read_file(const char* filename, char* buf, int buf_size);


// ======== Parsing ========= //

typedef enum {
    SYM_NONE, 
    SYM_LITERAL, 
    SYM_LABEL, 
    SYM_INSTR
} TokenType;


typedef struct 
{
    TokenType type;
    char      token_str[LEXER_TOKEN_BUF_SIZE];
} Token;


Token* create_token(void);
void   init_token(Token* token);

typedef struct
{
    // position info
    int  cur_pos;
    int  cur_line;
    char cur_char;

    // Token buffers
    char token_buf[LEXER_TOKEN_BUF_SIZE];
    int  token_buf_ptr;

    // address bookkeeping
    int text_addr;
    int data_addr;

} Lexer;


Lexer* create_lexer(void);
void   destroy_lexer(Lexer* lexer);


// Move through source
void lex_advance(Lexer* lexer, const char* src, size_t src_size);
void lex_scan_token(Lexer* lexer, const char* src, size_t src_size);
void lex_next_token(Lexer* lexer, Token* token, const char* src, size_t src_size);


int lex_file(Lexer* lexer, const char* src, size_t src_size);


#endif /*__EMU_ASSEM_H*/
