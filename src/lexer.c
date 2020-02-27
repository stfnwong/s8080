/*
 * LEXER
 * Lexer for 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

/*
 * symbol_create()
 */
Symbol* symbol_create(void)
{
    Symbol* s;

    s = malloc(sizeof(*s));
    return (!s) ? NULL : s;
}

/*
 * symbol_init()
 */
void symbol_init(Symbol* s)
{
    s->addr = 0;
    memset(s->sym, 0, MAX_SYM_LEN);
}

/*
 * symbol_copy()
 */
void symbol_copy(Symbol* dst, Symbol* src)
{
    if(dst == NULL || src == NULL)
        return;
    dst->addr = src->addr;
    //memcpy(dst->sym, src->sym, MAX_SYM_LEN);
    strncpy(dst->sym, src->sym, strlen(src->sym));
}

/*
 * symbol_print()
 */
void symbol_print(Symbol* s)
{
    fprintf(stdout, " 0x%04X : %s\n", s->addr, s->sym);
}

// ================ SYMBOL TABLE ================ //

/*
 * symbol_table_create()
 */
SymbolTable* symbol_table_create(int size)
{
    SymbolTable* table;

    table = malloc(sizeof(*table));
    if(!table)
        return NULL;

    table->size = 0;
    table->max_size = size;
    table->entries = malloc(sizeof(*table->entries) * table->max_size);
    if(!table->entries)
        return NULL;

    for(int i = 0; i < table->max_size; ++i)
    {
        table->entries[i] = malloc(sizeof(*table->entries[i]));
        if(!table->entries[i])
        {
            free(table->entries);
            free(table);
            return NULL;
        }
        table->entries[i] = symbol_create();
    }

    return table;
}

/*
 * symbol_table_destroy()
 */
void symbol_table_destroy(SymbolTable* table)
{
    if(table == NULL)
        free(table);
    else
    {
        for(int i = 0; i < table->max_size; ++i)
            free(table->entries[i]);

        free(table->entries);
        free(table);
    }
}

/*
 * symbol_table_add_sym()
 */
int symbol_table_add_sym(SymbolTable* table, Symbol* s)
{
    if(table->size == table->max_size)
        return -1;

    symbol_copy(table->entries[table->size], s);
    table->size++;
    //table->entries[table->size] =

    return 0;
}


/*
 * symbol_table_full()
 */
int symbol_table_full(SymbolTable* s)
{
    return (s->size == s->max_size) ? 1 : 0;
}
/*
 * symbol_table_empty()
 */
int symbol_table_empty(SymbolTable* s)
{
    return (s->size == 0) ? 1 : 0;
}

/*
 * symbol_table_get_idx()
 */
Symbol* symbol_table_get_idx(SymbolTable* table, int idx)
{
    if(idx < 0 || idx > table->max_size || idx > table->size)
        return NULL;

    return table->entries[idx];
}

/*
 * symbol_table_get_tr()
 */
Symbol* symbol_table_get_str(SymbolTable* table, char* str, int len)
{
    // For now just linear search
    // TODO : replace with heap?
    for(int s = 0; s < table->size; ++s)
    {
        if(strncmp(table->entries[s]->sym, str, len) == 0)
        {
            return table->entries[s];
        }
    }

    return NULL;
}


// ================ LEXER ================ //
Lexer* lexer_create(void)
{
    Lexer* lexer;

    lexer = malloc(sizeof(*lexer));
    if(!lexer)
        goto LEXER_END;

    // src params 
    lexer->src             = NULL;
    lexer->src_len         = 0;
    // init params
    lexer->cur_pos         = 0;
    lexer->cur_line        = 1;
    lexer->cur_col         = 1;
    lexer->cur_char        = '\0';
    lexer->token_buf_ptr   = 0;
    lexer->text_addr       = 0;
    lexer->data_addr       = 0;
    lexer->text_start_addr = 0;
    // Make the token buffer equal to an empty string
    lexer->token_buf[0]  = '\0';

    // misc settings
    lexer->verbose = 0;

    // Allocate the line info structure
    lexer->text_seg = line_info_create();
    if(!lexer->text_seg)
        goto LEXER_END;

    //lexer->data_seg = data_segment_create(5);
    //if(!lexer->data_seg)
    //    goto LEXER_END;

    // Don't allocate the source info object here, rather we
    // create it once we've read the source in and can guess at the 
    // number of lines we will need
    lexer->source_repr = NULL;
    // Also don't allocate any memory for the SymbolTable yet
    lexer->sym_table = NULL;

    lexer->op_table = opcode_table_create();
    if(!lexer->op_table)
        goto LEXER_END;
    lexer->dir_table = opcode_table_create_dir();
    if(!lexer->dir_table)
        goto LEXER_END;

LEXER_END:
    if(!lexer || !lexer->text_seg || !lexer->op_table)
    {
        fprintf(stderr, "[%s] failed to allocate memory for Lexer\n", __func__);
        return NULL;
    }

    return lexer;
}

/*
 * lexer_destroy()
 */
void lexer_destroy(Lexer* lexer)
{
    if(lexer->source_repr != NULL)
        source_info_destroy(lexer->source_repr);
    if(lexer->sym_table != NULL)
        symbol_table_destroy(lexer->sym_table);
    line_info_destroy(lexer->text_seg);
    opcode_table_destroy(lexer->op_table);
    free(lexer->src);
    free(lexer);
}

/*
 * lex_read_file()
 */
int lex_read_file(Lexer* lexer, const char* filename)
{
    size_t nread;
    FILE* fp;

    fp = fopen(filename, "r");
    if(!fp)
    {
        fprintf(stderr, "[%s] failed to open file [%s]\n",
                __func__, filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    lexer->src_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if(lexer->src != NULL)
        free(lexer->src);

    lexer->src = malloc(sizeof(char) * lexer->src_len);
    if(!lexer->src)
    {
        fprintf(stderr, "[%s] failed to allocate memory for lexer->src\n", __func__);
        fclose(fp);
        return -1;
    }

    do
    {
        nread = fread(lexer->src, 1, lexer->src_len, fp);
        if(ferror(fp))
        {
            fprintf(stderr, "[%s] error reading from file [%s]\n",
                    __func__, filename);
            fclose(fp);
            return -1;
        }
    } while(nread > 0);
    fclose(fp);

    // Also try to guess the number of lines we need. We just count the
    // number of newline chars and use that since this number is 
    // guaranteed to be big enough to hold what we need.
    int num_lines = 0;

    for(int cur_pos = 0; cur_pos < lexer->src_len; ++cur_pos)
    {
        if(lexer->src[cur_pos] == '\n')
            num_lines++;
    }

    // Create a new source representation for that number of lines
    if(lexer->source_repr != NULL)
        source_info_destroy(lexer->source_repr);
    lexer->source_repr = source_info_create(num_lines);
    if(!lexer->source_repr)
    {
        fprintf(stderr, "[%s] failed to create new SourceInfo for lexer with %d entries\n", __func__, num_lines);
        return -1;
    }

    // Create a symbol table with the same number of entries
    if(lexer->sym_table != NULL)
        symbol_table_destroy(lexer->sym_table);
    lexer->sym_table = symbol_table_create(num_lines);
    if(!lexer->sym_table)
    {
        fprintf(stderr, "[%s] failed to create new SymbolTable for lexer with %d entries\n", __func__, num_lines);
        return -1;
    }
    
    return 0;
}

// ==== Source motions ===== //

/*
 * lex_advance()
 */
void lex_advance(Lexer* lexer)
{
    lexer->cur_pos++;
    lexer->cur_col++;
    if(lexer->cur_pos >= lexer->src_len)
        lexer->cur_char = '\0';
    else
        lexer->cur_char = lexer->src[lexer->cur_pos];

    if(lexer->cur_char == '\n')
    {
        lexer->cur_line++;
        lexer->cur_col = 1;
    }
}

/*
 * lex_check_comma()
 */
int lex_check_comma(Lexer* lexer)
{
    return (lexer->src[lexer->cur_pos+1] == ',') ? 1 : 0;
}

/*
 * lex_is_whitespace()
 */
int lex_is_whitespace(const char c)
{
    if(c == ' ' || c == '\t' || c == '\n')
        return 1;
    return 0;
}

/*
 * lex_is_comment()
 */
int lex_is_comment(const char c)
{
    if(c == ';' || c == '#')
        return 1;
    return 0;
}

/*
 * lex_skip_whitespace()
 */
void lex_skip_whitespace(Lexer* lexer)
{
    while(lex_is_whitespace(lexer->src[lexer->cur_pos]))
        lex_advance(lexer);
}

/*
 * lex_skip_comment()
 */
void lex_skip_comment(Lexer* lexer)
{
    while(lexer->src[lexer->cur_pos] != '\n')
        lex_advance(lexer);

    // advance one more to get the line count up
    lex_advance(lexer);
}

/*
 * lex_set_text_start_addr()
 */
void lex_set_text_start_addr(Lexer* lexer, int addr)
{
    lexer->text_start_addr = addr;
}

/*
 * lex_text_addr_incr()
 */
void lex_text_addr_incr(Lexer* lexer, int instr_size)
{
    lexer->text_addr += 4 * instr_size;
}

/*
 * lex_data_addr_incr()
 */
void lex_data_addr_incr(Lexer* lexer)
{
    lexer->data_addr += 4;
}

// ==== Token Handling ===== //

/*
 * lex_scan_token()
 */
void lex_scan_token(Lexer* lexer)
{
    lexer->token_buf_ptr = 0;
    lex_skip_whitespace(lexer);
    while(lexer->token_buf_ptr < TOKEN_BUF_SIZE-1)
    {
        if(lexer->cur_char == ' ')      // space
            break;
        if(lexer->cur_char == '\n')     // newline
            break;
        if(lexer->cur_char == '\t')     // tab
            break;
        if(lexer->cur_char == '!')      // comment
            break;
        if(lexer->cur_char == ';')      // comment
            break;
        if(lexer->cur_char == '#')      // comment
            break;
        if(lexer->cur_char == ',')      // seperator
            break;
        if(lexer->cur_char == ':')      // end of label
        {
            lex_advance(lexer);
            break;
        }
        lexer->token_buf[lexer->token_buf_ptr] = lexer->cur_char;
        lex_advance(lexer);
        lexer->token_buf_ptr++;
    }

    lexer->token_buf[lexer->token_buf_ptr] = '\0';
    // move the cursor forward by one if we landed on a seperator
    if(lexer->cur_char == ',' || lexer->cur_char == ':' || lexer->cur_char == ' ')
        lex_advance(lexer);
}

/*
 * lex_extract_literal()
 */
int lex_extract_literal(Lexer* lexer, Token* token)
{
    int tok_ptr;
    int literal;
    char* end;

    //tok_ptr = lexer->cur_char;
    tok_ptr = 0;
    while(isdigit(lexer->token_buf[tok_ptr]))
        tok_ptr++;

    if(tok_ptr == 0)        // we didn't move
    {
        token->type = SYM_NONE;
        return 0;
    }

    // TODO : do we need to check for comments here?

    token->type = SYM_LITERAL;
    strncpy(token->token_str, lexer->token_buf, tok_ptr);
    token->token_str_len = tok_ptr;

    // Check if the last character is an 'H'
    if(lexer->token_buf[tok_ptr] == 'H' || 
       lexer->token_buf[tok_ptr] == 'h')
    {
        literal = (int) strtol(lexer->token_buf, &end, 16);
    }
    else
        literal = (int) strtol(lexer->token_buf, &end, 10);

    if(lexer->verbose)
    {
        fprintf(stdout, "[%s] got literal %d (0x%02X)\n", 
                __func__, literal, literal);
    }

    return literal;
}

/*
 * lex_next_string()
 */
void lex_next_string(Lexer* lexer, Token* token)
{
    while(lexer->token_buf_ptr < TOKEN_BUF_SIZE-1)
    {
        if(lexer->cur_char == '"')      // the end of this string
        {
            lex_advance(lexer);
            break;                      
        }
        if(lexer->cur_char == EOF)      // got to the end of the file
            break;
        if(lexer->cur_char == '\0')     // got a null
            break;

        lexer->token_buf[lexer->token_buf_ptr] = lexer->cur_char;
        lex_advance(lexer);
        lexer->token_buf_ptr++;
    }

    lexer->token_buf[lexer->token_buf_ptr] = '\0';
    // move the cursor forward by one if we landed on a seperator
    if(lexer->cur_char == ',' || lexer->cur_char == ':' || lexer->cur_char == ' ')
        lex_advance(lexer);
}

/*
 * lex_next_token()
 */
void lex_next_token(Lexer* lexer, Token* token)
{
    //int error = 0;
    Opcode opcode;
    // lex the next token, this places a new string into lexer->token_buf
    lex_scan_token(lexer);

    token_init(token);
    // Now check the token in the buffer
    if(isdigit(lexer->token_buf[0]))
    {
        token->type = SYM_LITERAL;
        goto TOKEN_END;
    }

    // Check for registers 
    if(strlen(lexer->token_buf) == 1)
    {
        if((strncmp(lexer->token_buf, "A", 1) == 0) || 
           (strncmp(lexer->token_buf, "B", 1) == 0) || 
           (strncmp(lexer->token_buf, "C", 1) == 0) || 
           (strncmp(lexer->token_buf, "D", 1) == 0) || 
           (strncmp(lexer->token_buf, "E", 1) == 0) ||
           (strncmp(lexer->token_buf, "H", 1) == 0) ||
           (strncmp(lexer->token_buf, "L", 1) == 0) ||
           (strncmp(lexer->token_buf, "M", 1) == 0) ||  // mem read
           (strncmp(lexer->token_buf, "S", 1) == 0))    // stack ptr
        {
            token->type = SYM_REG;
            token->token_str_len = 1;
            goto TOKEN_END;
        }

        if(lexer->token_buf[0] == ',')
        {
            token->type = SYM_COMMA;
            token->token_str_len = 1;
            goto TOKEN_END;
        }
    }
    else if(strlen(lexer->token_buf) == 3)
    {
        if(strncmp(lexer->token_buf, "PSW", 3) == 0)
        {
            token->type = SYM_REG;
            token->token_str_len = 1;
            goto TOKEN_END;
        }
    }

    // Check for strings
    if(lexer->token_buf[0] == '"')
    {
        lex_next_string(lexer, token);
        token->type = SYM_STRING;
        goto TOKEN_END;
    }

    // Check for directives. In this implementation we don't have 
    // leading '.' characters before a directive.
    opcode_init(&opcode);
    opcode_table_find_mnemonic(
            lexer->dir_table,
            &opcode,
            lexer->token_buf
    );
    if(opcode.instr != DIR_INVALID)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] got directive %d [%s]\n", 
                    __func__, 
                    opcode.instr, 
                    LEX_DIRECTIVES[opcode.instr].mnemonic);
        }
        token->type = SYM_DIRECTIVE;
        token->token_str_len = (int) strlen(lexer->token_buf);
        goto TOKEN_END;
    }

    // Check if the token is an instruction
    opcode_init(&opcode);
    opcode_table_find_mnemonic(
            lexer->op_table, 
            &opcode, 
            lexer->token_buf
    );
    if(opcode.instr != LEX_INVALID)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] got opcode %d [%s]\n", 
                    __func__, 
                    opcode.instr, 
                    LEX_INSTRUCTIONS[opcode.instr].mnemonic);
        }
        token->type = SYM_INSTR;
        token->token_str_len = (int) strlen(lexer->token_buf);
        goto TOKEN_END;
    }

    // Since we cant match anything, we treat as a label
    // NOTE: I have considered that this is not the fastest way to do
    // this, since labels come first and we first have to fail through
    // all the other possibilities before declaring a label. 

    // Must be a label
    token->type = SYM_LABEL;

TOKEN_END:
    // If this is a label, then null-out any trailing ':' characters
    if(token->type == SYM_LABEL)
    {
        int copy_size;
        if(lexer->token_buf[lexer->token_buf_ptr-1] == ':')
            copy_size = lexer->token_buf_ptr-1;
        else
            copy_size = lexer->token_buf_ptr;
        strncpy(token->token_str, lexer->token_buf, copy_size);
        token->token_str[copy_size+1] = '\0';
        token->token_str_len = copy_size + 1;
    }
    else
    {
        strcpy(token->token_str, lexer->token_buf);
        token->token_str[lexer->token_buf_ptr+1] = '\0';
        token->token_str_len = lexer->token_buf_ptr + 1;
    }

    if(lexer->verbose)
    {
        fprintf(stdout, "[%s]  (line %d:%d) got token [%s] of type %s \n",
               __func__, 
               lexer->cur_line, 
               lexer->cur_col, 
               token->token_str,
               TOKEN_TYPE_TO_STR[token->type]
        );
    }
}

/*
 * lex_parse_one_reg()
 */
int lex_parse_one_reg(Lexer* lexer, Token* token)
{
    int status = 0;

    if(token->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[token->type]
            );
        }
        status = -1;
    }
    lexer->text_seg->reg[0] = reg_char_to_code(token->token_str[0]);

    return status;
}

/*
 * lex_parse_two_reg()
 */
int lex_parse_two_reg(Lexer* lexer, Token* tok_a, Token* tok_b)
{
    int status = 0;

    if(tok_a->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[tok_a->type]
            );
        }
        status = -1;
    }

    if(tok_b->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[tok_b->type]
            );
        }
        status = -1;
    }

    lexer->text_seg->reg[0] = reg_char_to_code(tok_a->token_str[0]);
    lexer->text_seg->reg[1] = reg_char_to_code(tok_b->token_str[0]);

    return status;
}

/*
 * lex_parse_reg_imm()
 */
int lex_parse_reg_imm(Lexer* lexer, Token* tok_a, Token* tok_b)
{
    int status = 0;

    if(tok_a->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[tok_a->type]
            );
        }
        status = -1;
    }

    // Second arg could also be a symbol to be resolved in next pass
    if(tok_b->type == SYM_LITERAL)
    {
        lexer->text_seg->reg[0]        = reg_char_to_code(tok_a->token_str[0]);
        lexer->text_seg->immediate     = lex_extract_literal(lexer, tok_b);
        lexer->text_seg->has_immediate = 1;
    }
    else if(tok_b->type == SYM_LABEL)
    {
        status = line_info_set_symbol_str(
                lexer->text_seg,
                tok_b->token_str,
                strlen(tok_b->token_str)
        );
    }
    else
    {
        fprintf(stdout, "[%s] line %d:%d, ERROR: expected immediate or label, got %s\n",
               __func__, lexer->cur_line, lexer->cur_col, 
               TOKEN_TYPE_TO_STR[tok_b->type]
        );
        status = -1;
    }

    return status;
}

/*
 * lex_parse_imm()
 */
int lex_parse_imm(Lexer* lexer, Token* tok)
{
    int status = 0;

    if(tok->type == SYM_LITERAL)
    {
        lexer->text_seg->immediate     = lex_extract_literal(lexer, tok);
        lexer->text_seg->has_immediate = 1;
    }
    else if(tok->type == SYM_LABEL)
    {
        status = line_info_set_symbol_str(
                lexer->text_seg,
                tok->token_str,
                strlen(tok->token_str)
        );
    }
    else
    {
        fprintf(stdout, "[%s] line %d:%d, ERROR: expected immediate or label, got %s\n",
               __func__, lexer->cur_line, lexer->cur_col, 
               TOKEN_TYPE_TO_STR[tok->type]
        );
        status = -1;
    }

    return status;
}

/*
 * lex_parse_jmp()
 */
int lex_parse_jmp(Lexer* lexer, Token* tok)
{
    int status = 0;

    if(tok->type == SYM_LABEL)
    {
        status = line_info_set_symbol_str(
                lexer->text_seg, 
                tok->token_str, 
                strlen(tok->token_str)
        );
    }
    else if(tok->type == SYM_LITERAL)
    {
        lexer->text_seg->immediate     = lex_extract_literal(lexer, tok);
        lexer->text_seg->has_immediate = 1;
    }
    else
    {
        fprintf(stdout, "[%s] line %d:%d ERROR: expected symbol or literal, got %s\n",
                __func__, 
                lexer->cur_line, 
                lexer->cur_col,
                TOKEN_TYPE_TO_STR[tok->type]
               );
        status = -1;
    }

    return status;
}

/*
 * lex_parse_data_arg()
 */
int lex_parse_data_arg(Lexer* lexer, Token* tok)
{
    int status;
    if(tok->type == SYM_LITERAL)
    {
        lexer->text_seg->immediate     = lex_extract_literal(lexer, tok);
        lexer->text_seg->has_immediate = 1;
        status = 0;
    }
    else if(tok->type == SYM_STRING)
    {
        status = line_info_set_byte_array(
                lexer->text_seg,
                (uint8_t*) tok->token_str,
                strlen(tok->token_str)
        );
    }
    else if(tok->type == SYM_LABEL)
    {
        status = line_info_set_symbol_str(
                lexer->text_seg,
                tok->token_str,
                strlen(tok->token_str)
        );
    }
    else
    {
        fprintf(stdout, "[%s] line %d:%d ERROR: expected string or literal, got %s\n",
                __func__, 
                lexer->cur_line, 
                lexer->cur_col,
                TOKEN_TYPE_TO_STR[tok->type]
               );
        status = -1;
    }

    return status;
}

/*
 * lex_parse_data()
 */
int lex_parse_data(Lexer* lexer, Token* tok)
{
    int status;
    int data_line;

    // once we go to a  new line we don't need to lex anymore tokens 
    // into the buffer
    data_line = lexer->cur_line;

    // deal with the current token
    status = lex_parse_data_arg(lexer, tok);
    if(status < 0)
        goto LEX_PARSE_DATA_END;
    
    while(lex_check_comma(lexer))
    {
        lex_next_token(lexer, tok);
        status = lex_parse_data_arg(lexer, tok);
        if(status < 0)
            goto LEX_PARSE_DATA_END;
        // TODO : actually we need to to be able to extend this
        // since the current implementation only writes the 
        // last byte

        // TODO: put a vector in here for byte array
        status = line_info_set_byte_array(
                lexer->text_seg,
                (uint8_t*) tok->token_str,
                strlen(tok->token_str)
        );
        if(status < 0)
            goto LEX_PARSE_DATA_END;
    }

LEX_PARSE_DATA_END:
    return status;
}

/*
 * lex_parse_string()
 */
int lex_parse_string(Lexer* lexer, Token* tok)
{
    int status = 0;

    if(tok->type != SYM_STRING)
    {
        fprintf(stdout, "[%s] line %d:%d ERROR: expected string, got %s\n",
                __func__, 
                lexer->cur_line, 
                lexer->cur_col,
                TOKEN_TYPE_TO_STR[tok->type]
               );
        status = -1;
    }
    status = line_info_set_byte_array(
            lexer->text_seg,
            (uint8_t*) tok->token_str,
            strlen(tok->token_str)
    );

    return status;
}


/*
 * lex_resolve_labels()
 */
void lex_resolve_labels(Lexer* lexer)
{
    // If there are no symbols, then nothing to do
    if(lexer->sym_table->size == 0)
        return;

    LineInfo* cur_line;
    Symbol* out_sym;
    // walk over the source repr, each time there is a label
    // look it up in the symbol table and insert the address of
    // that symbol in the immediate field
    for(int l = 0; l < lexer->source_repr->size; ++l)
    {
        cur_line = source_info_get_idx(lexer->source_repr, l);
        if(cur_line->symbol_str_len > 0)
        {
            // lookup the label in the symbol table
            out_sym = symbol_table_get_str(
                    lexer->sym_table, 
                    cur_line->symbol_str,
                    cur_line->symbol_str_len
            );
            // if the symbol is valid then update the line
            if(out_sym != NULL)
            {
                lexer->source_repr->buffer[l]->immediate = out_sym->addr;
                lexer->source_repr->buffer[l]->has_immediate = 1;
            }
        }
    }
}

/*
 * lex_line()
 */
int lex_line(Lexer* lexer)
{
    int status = 0;
    int instr_size = 1;
    Opcode cur_opcode;
    Token cur_token;
    Token tok_a, tok_b;
    Symbol cur_sym;

    token_init(&tok_a);
    token_init(&tok_b);
    token_init(&cur_token);
    line_info_init(lexer->text_seg);

    lex_next_token(lexer, &cur_token);

    // Handle labels
    if(cur_token.type == SYM_LABEL)
    {
        status = line_info_set_label_str(
                lexer->text_seg,
                cur_token.token_str,
                strlen(cur_token.token_str)
        );
        if(status < 0)
            goto LEX_LINE_END;

        // make a symbol object for this label
        cur_sym.addr = lexer->text_addr;
        strncpy(cur_sym.sym, cur_token.token_str, cur_token.token_str_len);

        // Add to symbol table
        status = symbol_table_add_sym(lexer->sym_table, &cur_sym);
        if(status < 0)
        {
            fprintf(stderr, "[%s] (line %d:%d) failed to insert symbol %s\n",
                    __func__, lexer->cur_line, lexer->cur_col, cur_token.token_str
            );
            goto LEX_LINE_END;
        }
        // Get the next token ready
        lex_next_token(lexer, &cur_token);
    }

    // Parse directives 
    if(cur_token.type == SYM_DIRECTIVE)
    {
        // TODO : in the refactor we should find a way to not have to do this (lookup the code) twice
        opcode_init(&cur_opcode);
        opcode_table_find_mnemonic(lexer->dir_table, &cur_opcode, cur_token.token_str);
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] (line %d:%d) lexing %s\n",
                    __func__, 
                    lexer->cur_line,
                    lexer->cur_col,
                    cur_opcode.mnemonic
            );
        }

        switch(cur_opcode.instr)
        {
            case DIR_END:
                fprintf(stdout, "[%s] got END\n", __func__);
                break;
            case DIR_ENDIF:
                fprintf(stdout, "[%s] got ENDIF\n", __func__);
                break;
            case DIR_ENDM:
                fprintf(stdout, "[%s] got ENDM\n", __func__);
                break;
            case DIR_IF:
                fprintf(stdout, "[%s] got IF\n", __func__);
                break;
            case DIR_MACRO:
                fprintf(stdout, "[%s] got MACRO\n", __func__);
                break;
            case DIR_ORG:
                fprintf(stdout, "[%s] got ORG\n", __func__);
                break;
            case DIR_SET:
                fprintf(stdout, "[%s] got SET\n", __func__);
                break;

            default:
                if(lexer->verbose)
                {
                    fprintf(stderr, "[%s] invalid directive with value %s\n", __func__, cur_opcode.mnemonic);
                }
                goto LEX_LINE_END;
        }

        if(status < 0)
        {
            fprintf(stderr, "[%s] failed to lex directive %s\n", __func__, cur_token.token_str);
            goto LEX_LINE_END;
        }

        lexer->text_seg->opcode->instr = cur_opcode.instr;
        strcpy(lexer->text_seg->opcode->mnemonic, cur_opcode.mnemonic);
    }

    // Parse instructions 
    if(cur_token.type == SYM_INSTR)
    {
        // TODO : in the refactor we should find a way to not have to do this (lookup the code) twice
        opcode_init(&cur_opcode);
        opcode_table_find_mnemonic(lexer->op_table, &cur_opcode, cur_token.token_str);

        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] (line %d:%d) lexing %s\n",
                    __func__, 
                    lexer->cur_line,
                    lexer->cur_col,
                    cur_opcode.mnemonic
            );
        }

        switch(cur_opcode.instr)
        {
            // Single register
            case LEX_ADC:
            case LEX_ADD:
            case LEX_ANA:
            case LEX_CMP:
            case LEX_DAD:
            case LEX_DCR:
            case LEX_INR:
            case LEX_INX:
            case LEX_LDAX:
            case LEX_ORA:
            case LEX_POP:
            case LEX_PUSH:
            case LEX_SBB:
            case LEX_STAX:
            case LEX_SUB:
            case LEX_XRA:
                lex_next_token(lexer, &cur_token);
                status = lex_parse_one_reg(lexer, &cur_token);
                instr_size = 1;
                break;

            case LEX_ACI:
            case LEX_ADI:
            case LEX_ANI:
            case LEX_CPI:
            case LEX_ORI:
            case LEX_SUI:
            case LEX_SBI:
                lex_next_token(lexer, &cur_token);
                status = lex_parse_imm(lexer, &cur_token);
                instr_size = 2;
                break;

            case LEX_MOV:
                // For MOV, we need two registers
                lex_next_token(lexer, &tok_a);
                lex_next_token(lexer, &tok_b);
                status = lex_parse_two_reg(lexer, &tok_a, &tok_b);
                instr_size = 1;
                break;

            case LEX_MVI:
                lex_next_token(lexer, &tok_a);
                lex_next_token(lexer, &tok_b);
                status = lex_parse_reg_imm(lexer, &tok_a, &tok_b);
                instr_size = 2;
                break;

            case LEX_PCHL:
                instr_size = 1;
                break;

            case LEX_LXI:
                instr_size = 3;
                fprintf(stdout, "[%s] yet to implement LXI...\n", __func__);
                break;

            // Control flow instructions
            // These are assembled into three bytes
            case LEX_JP:
            case LEX_JMP:
            case LEX_JC:
            case LEX_JNC:
            case LEX_JZ:
                lex_next_token(lexer, &tok_a);  // should be literal or label
                status = lex_parse_jmp(lexer, &tok_a);
                instr_size = 3;
                break;

            // subroutine call instructions 
            case LEX_CALL:
            case LEX_CNZ:
            case LEX_CM:
            case LEX_CP:
            case LEX_CPE:
            case LEX_CPO:
            case LEX_CZ:
                lex_next_token(lexer, &tok_a);
                status = lex_parse_data_arg(lexer, &tok_a);
                fprintf(stdout, "[%s] status for lex_parse_data_arg() for call-type instruction = %d\n", __func__, status);
                instr_size = 2;
                break;

            // subroutine return instructions
            case LEX_RET:
            case LEX_RC:
            case LEX_RNC:
            case LEX_RZ:
            case LEX_RM:
            case LEX_RP:
            case LEX_RPE:
            case LEX_RPO:
                instr_size = 1;
                break;

            // data instructions 
            case LEX_DB:
            case LEX_DW:
                lex_next_token(lexer, &tok_a);
                status = lex_parse_data(lexer, &tok_a);
                lex_next_token(lexer, &tok_a);
                // something like 
                // while(token is a comma)
                //  lex_next_token()
                //  parse_string(with new token)
                // TODO : Note that DB can accept comma-delimited arg lists
                instr_size = 1;
                break;

            case LEX_DS:
                // Since this just reserves space, all the real 
                // work is done in the assembler component
                instr_size = 1;
                break;

            default:
                if(lexer->verbose)
                {
                    fprintf(stderr, "[%s] invalid opcode with value %02X\n", __func__, cur_opcode.instr);
                }
                goto LEX_LINE_END;
        }

        if(status < 0)
        {
            fprintf(stderr, "[%s] failed to lex instruction %s\n", __func__, cur_token.token_str);
            goto LEX_LINE_END;
        }

        lexer->text_seg->opcode->instr = cur_opcode.instr;
        strcpy(lexer->text_seg->opcode->mnemonic, cur_opcode.mnemonic);
    }

LEX_LINE_END:
    if(status < 0)
    {
        fprintf(stdout, "[%s] something went wrong...\n", __func__);
        lexer->text_seg->error = 1;
    }
    lex_text_addr_incr(lexer, instr_size);
    lexer->text_seg->addr = lexer->text_addr;

    status = source_info_add_line(lexer->source_repr, lexer->text_seg);
    if(status < 0)
    {
        fprintf(stderr, "[%s] (line %d:%d) failed to update source info\n",
                __func__, lexer->cur_line, lexer->cur_col);
    }

    return status;
}


/*
 * lex_all()
 */
int lex_all(Lexer* lexer)
{
    // TODO : some init phase?
    int status;
    lexer->cur_char = lexer->src[0];
    lexer->text_addr = lexer->text_start_addr;

    while(lexer->cur_pos < lexer->src_len)
    {
        // eat comments
        if(lex_is_comment(lexer->cur_char))
        {
            lex_skip_comment(lexer);
            continue;
        }

        // eat whitespace
        if(lex_is_whitespace(lexer->cur_char))
        {
            lex_advance(lexer);
            continue;
        }
        // This is a valid line, so start trying to get tokens together
        status = lex_line(lexer);
        if(status < 0)
            return status;
    }

    // Resolve label addresses
    lex_resolve_labels(lexer);

    return 0;
}

int lex_write_repr(Lexer* lexer, const char* filename)
{
    if(lexer->source_repr == NULL)
        return -1;

    FILE* fp;

    fp = fopen(filename, "wb");
    if(!fp)
    {
        fprintf(stderr, "[%s] failed to open file %s for writing\n",
                __func__, filename);
        return -1;
    }


    return 0;
}
