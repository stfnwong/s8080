/*
 * SOURCE
 * Stuff for dealing with assembly source
 *
 * Stefan Wong 2020
 */

#ifndef __S8000_SOURCE_H
#define __S8000_SOURCE_H

#define TOKEN_BUF_SIZE 64
#define LINE_INFO_NUM_REG 2
// Maximum sizes for label strings, symbol strings
#define SOURCE_INFO_MAX_LABEL_LEN  32
#define SOURCE_INFO_MAX_SYMBOL_LEN 32

#include "opcode.h"
#include "list.h"

// Register names 
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

// String conversion arrays
extern const char* TOKEN_TYPE_TO_STR[9];
extern const char* REG_TYPE_TO_STR[11];

// Text segment
typedef struct 
{
    Opcode*   opcode;   // make this not a reference...
    char      label_str[SOURCE_INFO_MAX_LABEL_LEN];
    char      symbol_str[SOURCE_INFO_MAX_SYMBOL_LEN];
    int       label_str_len;
    int       symbol_str_len;
    // Position
    int       line_num;
    int       addr;
    // arguments 
    int       has_immediate;
    int       immediate;
    RegType   reg[LINE_INFO_NUM_REG];
    // Any raw bytes that were written to the program output
    ByteList* byte_list;        // TODO : this is going to be a hassle....
    // error info
    int       error;
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
void      line_info_delete_bytes(LineInfo* info);
int       line_info_num_bytes(LineInfo* info);
int       line_info_append_byte_array(LineInfo* info, uint8_t* data, int len, uint16_t addr);

/*
 * reg_char_to_code()
 * Convert a character to a register enum code
 */
uint8_t   reg_char_to_code(char r);

// ==== Buffer for LineInfo Structures
typedef struct SourceInfo SourceInfo;

struct SourceInfo
{
    LineInfo** buffer;      // Need pointers here, alloc and copy each time?
    int        size;
    int        capacity;
};

// TODO : this might not work as well as I hoped, as the size of each LineInfo is not the same....
// But wait a minute, if that's the case then how does the current implementation work?
SourceInfo* source_info_create(int num_lines);
void        source_info_destroy(SourceInfo* info);
void        source_info_extend(SourceInfo* info, int ext_size);
void        source_info_add_line(SourceInfo* info, LineInfo* line); 
int         source_info_edit_line(SourceInfo* info, LineInfo* line, int idx);
LineInfo*   source_info_get_idx(SourceInfo* info, int idx);
SourceInfo* source_info_clone(SourceInfo* src);
int         source_info_empty(SourceInfo* info);
int         source_info_size(SourceInfo* info);
int         source_info_capacity(SourceInfo* info);

// TODO : init method which resets sourceinfo?

// ======== TOKEN ======== //
typedef enum {
    SYM_NONE, 
    SYM_LITERAL, 
    SYM_LABEL, 
    SYM_DIRECTIVE,
    SYM_INSTR,
    SYM_REG,
    SYM_STRING,
    SYM_COMMA,
    SYM_EOF
} TokenType;

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

#endif /*__S8000_SOURCE_H*/
