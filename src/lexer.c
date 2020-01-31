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

// ================ LINE INFO ================ //
LineInfo* line_info_create(void)
{
    LineInfo* info;

    info = malloc(sizeof(*info));
    if(!info)
        goto INFO_END;

    info->opcode = malloc(sizeof(*info->opcode));
    if(!info->opcode)
        goto INFO_END;

    line_info_init(info);
    //init_opcode(info->opcode);      // TODO : name change to opcode_init

INFO_END:
    if(!info || !info->opcode)
    {
        fprintf(stderr, "[%s] failed to allocate memory while creating LineInfo\n", __func__);
        return NULL;
    }

    return info;
}

/*
 * line_info_destroy()
 */
void line_info_destroy(LineInfo* info)
{
    free(info->opcode);
    free(info);
}

/*
 * line_info_init()
 */
void line_info_init(LineInfo* info)
{
    init_opcode(info->opcode);
    info->line_num = 0;
    info->addr     = 0;
    for(int a = 0; a < 3; ++a)
        info->args[a] = '\0';
}


/*
 * data_segment_create()
 */
DataSegment* data_segment_create(int size)
{
    DataSegment* segment;

    segment = malloc(sizeof(*segment));
    if(!segment)
        goto SEGMENT_END;

    segment->data = malloc(size * sizeof(uint8_t));
    if(!segment->data)
        goto SEGMENT_END;

    segment->data_size = size;
    segment->addr      = 0;

SEGMENT_END:
    if(!segment || ! segment->data)
    {
        fprintf(stderr, "[%s] failed to allocate memory for DataSegment\n", __func__);
        return NULL;
    }

    return segment;
}

/*
 * data_segment_destroy()
 */
void data_segment_destroy(DataSegment* segment)
{
    free(segment->data);
    free(segment);
}


// ================ FILE HANDLING ================ //

/*
 * lex_get_file_size()
 */
int lex_get_file_size(const char* filename)
{
    int fsize;
    FILE* fp;

    fp = fopen(filename, "r");
    if(!fp)
    {
        fprintf(stderr, "[%s] failed to open file [%s]\n",
                __func__, filename);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fclose(fp);

    return fsize;
}

/*
 * lex_read_file()
 * Note that we expect the caller to have allocated enough memory here 
 * to hold the contents of the file.
 */
int lex_read_file(const char* filename, char* buf, int buf_size)
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

    do
    {
        nread = fread(buf, 1, buf_size, fp);
        if(ferror(fp))
        {
            fprintf(stderr, "[%s] error reading from file [%s]\n",
                    __func__, filename);
            fclose(fp);
            return -1;
        }
    } while(nread > 0);
    fclose(fp);

    return 0;
}

// ================ TOKEN ================ //
/*
 * create_token()
 */
Token* create_token(void)
{
    Token* token;

    token = malloc(sizeof(*token));
    if(!token)
    {
        fprintf(stderr, "[%s] failed to allocate memory for token\n",
                __func__);
        return NULL;
    }
    token->type         = SYM_NONE;
    token->token_str[0] = '\0';     // or memset()?

    return token;
}

/*
 * init_token()
 */
void init_token(Token* token)
{
    token->type = SYM_NONE;
    token->token_str[0] = '\0';
}

/*
 * destroy_token()
 */
void destroy_token(Token* token)
{
    free(token);
}


// ================ LEXER ================ //
Lexer* create_lexer(void)
{
    Lexer* lexer;

    lexer = malloc(sizeof(*lexer));
    if(!lexer)
        goto LEXER_END;

    // init params
    lexer->cur_pos       = 0;
    lexer->cur_line      = 1;
    lexer->cur_col       = 1;
    lexer->cur_char      = '\0';
    lexer->token_buf_ptr = 0;
    lexer->text_addr     = 0;
    lexer->data_addr     = 0;
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

LEXER_END:
    if(!lexer || !lexer->text_seg)      // TODO : add data seg
    {
        fprintf(stderr, "[%s] failed to allocate memory for Lexer\n", __func__);
        return NULL;
    }

    return lexer;
}

void destroy_lexer(Lexer* lexer)
{
    free(lexer);
}

// ==== Source motions ===== //

/*
 * lex_advance()
 */
void lex_advance(Lexer* lexer, const char* src, size_t src_size)
{
    lexer->cur_pos++;
    lexer->cur_col++;
    if(lexer->cur_pos >= src_size)
        lexer->cur_char = '\0';
    else
        lexer->cur_char = src[lexer->cur_pos];

    if(lexer->cur_char == '\n')
    {
        lexer->cur_line++;
        lexer->cur_col = 1;
    }
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
void lex_skip_whitespace(Lexer* lexer, const char* src, size_t src_size)
{
    while(lex_is_whitespace(src[lexer->cur_pos]))
        lex_advance(lexer, src, src_size);
}

/*
 * lex_skip_comment()
 */
void lex_skip_comment(Lexer* lexer, const char* src, size_t src_size)
{
    while(src[lexer->cur_pos] != '\n')
        lex_advance(lexer, src, src_size);
    // advance one more to get the line count up
    lex_advance(lexer, src, src_size);
}

// ==== Token Handling ===== //

/*
 * lex_scan_token()
 */
void lex_scan_token(Lexer* lexer, const char* src, size_t src_size)
{
    lexer->token_buf_ptr = 0;
    lex_skip_whitespace(lexer, src, src_size);
    while(lexer->token_buf_ptr < LEXER_TOKEN_BUF_SIZE-1)
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
            lex_advance(lexer, src, src_size);
            break;
        }

        lexer->token_buf[lexer->token_buf_ptr] = lexer->cur_char;
        lex_advance(lexer, src, src_size);
        lexer->token_buf_ptr++;
    }

    lexer->token_buf[lexer->token_buf_ptr] = '\0';
    // move the cursor forward by one if we landed on a seperator
    if(lexer->cur_char == ',' || lexer->cur_char == ':' || lexer->cur_char == ' ')
        lex_advance(lexer, src, src_size);
}

/*
 * lex_extract_literal()
 */
int lex_extract_literal(Lexer* lexer, Token* token, const char* src, size_t src_size)
{
    int tok_ptr;
    int literal;
    char* end;

    tok_ptr = lexer->cur_char;
    while(isdigit(lexer->token_buf[tok_ptr]))
        tok_ptr++;

    if(tok_ptr == lexer->cur_char)      // we didn't move
    {
        token->type = SYM_NONE;
        return 0;
    }

    token->type = SYM_LITERAL;
    strncpy(token->token_str, src[lexer->cur_char], tok_ptr);
    // Check if the last character is an 'H'
    if(lexer->token_buf[tok_ptr+1] == 'H' || 
       lexer->token_buf[tok_ptr+1] == 'h')
    {
        literal = (int) strtol(lexer->token_buf, &end, 16);
    }
    else
        literal = (int) strtol(lexer->token_buf, &end, 10);

    return literal;
}

/*
 * lex_next_token()
 */
void lex_next_token(Lexer* lexer, Token* token, const char* src, size_t src_size)
{
    int error = 0;
    // lex the next token, this places a new string into lexer->token_buf
    lex_scan_token(lexer, src, src_size);

    init_token(token);

    // TODO : later on allow for strings, chars

    // Now check the token in the buffer
    if(isdigit(lexer->token_buf[0]))
    {
        int literal = lex_extract_literal(lexer, token, src, src_size);
        if(token->type == SYM_NONE)
            error = 1;      // TODO : what am I actually going to do with this?

        goto TOKEN_END;
    }

TOKEN_END:
    if(lexer->verbose)
    {
        fprintf(stdout, "[%s]  line %d:%d, got token [%s] with value [%s]\n",
               __func__, lexer->cur_line, lexer->cur_col, 
               lexer->token_buf, token->token_str
        );
    }
}


/*
 * lex_line()
 */
void lex_line(Lexer* lexer, const char* src, size_t src_size)
{

}





// TODO : this will be the entry point for the lexer
int lex_file(Lexer* lexer, const char* src, size_t src_size)
{
    Token* cur_token = create_token();
    if(!cur_token)
    {
        fprintf(stdout, "[%s] failed to allocate memory for cur_token\n", __func__);
        return -1;
    }
    init_token(cur_token);

    while(lexer->cur_pos < src_size)
    {
        // eat whitespace
        if(lex_is_whitespace(lexer->cur_char))
        {
            lex_advance(lexer, src, src_size);
            continue;
        }

        // eat comments
        if(lex_is_comment(lexer->cur_char))
        {
            lex_skip_comment(lexer, src, src_size);
            continue;
        }

        // This is a valid line, so start trying to get tokens together
        lex_line(lexer, src, src_size);
    }

    // TODO : labels, label resolution

    return 0;
}
