/*
 * LEXER
 * Lexer for 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#ifndef __EMU_LEXER_H
#define __EMU_LEXER_H

#define MAX_SYM_LEN 32
#define LEX_TEXT_INCR 1
#define LEX_DATA_INCR 1

#include <stdint.h>
#include "opcode.h"
#include "source.h"

/*
 * Symbol object
 */
typedef struct
{
    int   addr;
    char  sym[MAX_SYM_LEN];
    //int   sym_len;      // 
} Symbol;

Symbol* symbol_create(void);
void    symbol_init(Symbol* s);
void    symbol_copy(Symbol* dst, Symbol* src);
void    symbol_print(Symbol* s);

/*
 * SymbolTable
 */
typedef struct
{
    Symbol** entries;
    int size;
    int max_size;
} SymbolTable;

// Don't bother with copying for now
SymbolTable* symbol_table_create(int size);
void         symbol_table_destroy(SymbolTable* table);
int          symbol_table_add_sym(SymbolTable* table, Symbol* s);
int          symbol_table_full(SymbolTable* s);
int          symbol_table_empty(SymbolTable* s);
Symbol*      symbol_table_get_idx(SymbolTable* table, int idx);
Symbol*      symbol_table_get_str(SymbolTable* table, char* str, int len);

/*
 * Lexer object
 */
typedef struct Lexer Lexer;

struct Lexer
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
    char  token_buf[TOKEN_BUF_SIZE];
    int   token_buf_ptr;
    // address bookkeeping
    int   text_addr;
    int   data_addr;
    int   text_start_addr;
    // current line information
    LineInfo*    text_seg;      
    SourceInfo*  source_repr;
    // Opcode table
    OpcodeTable* op_table;
    OpcodeTable* dir_table;
    // Symbol table
    SymbolTable* sym_table;
    // misc settings
    int verbose;
};

Lexer* lexer_create(void);
void   lexer_destroy(Lexer* lexer);
int    lex_read_file(Lexer* lexer, const char* filename);

// Move through source
int    lex_is_whitespace(const char c);
int    lex_is_comment(const char c);
void   lex_advance(Lexer* lexer);
int    lex_check_comma(Lexer* lexer); // look-ahead to see if there is a comma
int    lex_src_end(Lexer* lexer);
void   lex_skip_whitespace(Lexer* lexer);
void   lex_skip_comment(Lexer* lexer);

// Update addreses
void   lex_set_text_start_addr(Lexer* lexer, int addr);
void   lex_text_addr_incr(Lexer* lexer, int instr_size);
void   lex_data_addr_incr(Lexer* lexer);

//  extract tokens
void   lex_scan_token(Lexer* lexer);
void   lex_next_string(Lexer* lexer, Token* token);
void   lex_next_token(Lexer* lexer, Token* token);  

// Parse instructions 
int    lex_parse_one_reg(Lexer* lexer, Token* token);
int    lex_parse_two_reg(Lexer* lexer, Token* tok_a, Token* tok_b);
int    lex_parse_reg_imm(Lexer* lexer, Token* tok_a, Token* tok_b);
int    lex_parse_imm(Lexer* lexer, Token* tok);
int    lex_parse_jmp(Lexer* lexer, Token* tok);
int    lex_parse_data_arg(Lexer* lexer, Token* tok);
int    lex_parse_data(Lexer* lexer, Token* tok);
int    lex_parse_string(Lexer* lexer, Token* tok);
//int    lex_parse_call(Lexer* lexer, Token* tok);

// Label resolution
void   lex_resolve_labels(Lexer* lexer);

// Lex a line in the source 
int    lex_line(Lexer* lexer);
int    lex_all(Lexer* lexer);
int    lex_write_repr(Lexer* lexer, const char* filename);

// Getters 
SourceInfo* lex_get_repr(Lexer* lexer);
SymbolTable* lex_get_symbol_table(Lexer* lexer);

// Misc 
void   lex_set_verbose(Lexer* lexer);
void   lex_clear_verbose(Lexer* lexer);
int    lex_verbose(Lexer* lexer);

#endif /*__EMU_LEXER_H*/
