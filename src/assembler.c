/*
 * ASSEMBLER
 * 8080 Assembler implementation.
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"


/*
 * asm_get_file_size()
 */
int asm_get_file_size(const char* filename)
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
 * asm_read_file()
 * Note that we expect the caller to have allocated enough memory here 
 * to hold the contents of the file.
 */
int asm_read_file(const char* filename, char* buf, int buf_size)
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
Token* create_token(void)
{
    Token* token;

    token = malloc(sizeof(Token));
    if(!token)
    {
        fprintf(stderr, "[%s] failed to allocate memory for token\n",
                __func__);
        return NULL;
    }
    token->type = SYM_NONE;
    token->token_str[0] = '\0';

    return token;
}


// ================ LEXER ================ //
Lexer* create_lexer(void)
{
    Lexer* lexer;

    lexer = malloc(sizeof(*lexer));
    if(!lexer)
    {
        fprintf(stderr, "[%s] failed to allocate memory for lexer\n",
                __func__);
        return NULL;
    }

    // init params
    lexer->cur_pos       = 0;
    lexer->cur_line      = 1;
    lexer->cur_char      = '\0';
    lexer->token_buf_ptr = 0;
    lexer->text_addr     = 0;
    lexer->data_addr     = 0;
    // Make the token buffer equal to an empty string
    lexer->token_buf[0]  = '\0';

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
    if(lexer->cur_pos >= src_size)
        lexer->cur_char = '\0';
    else
        lexer->cur_char = src[lexer->cur_pos];

    if(lexer->cur_char == '\n')
        lexer->cur_line++;
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


/*
 * lex_scan_token()
 */
void lex_scan_token(Lexer* lexer, const char* src, size_t src_size)
{
    int idx;

    idx = 0;

    while(idx < LEXER_TOKEN_BUF_SIZE)
    {
        if(lexer->cur_char == ' ')      // space
            break;
        if(lexer->cur_char == '\n')     // newline
            break;
        if(lexer->cur_char == '!')      // newline
            break;
        if(lexer->cur_char == ';')      // comment
            break;
        if(lexer->cur_char == '#')      // comment
            break;
        if(lexer->cur_char == ',')      // seperator
            break;
        if(lexer->cur_char == ':')      // end of label
            break;

        lexer->token_buf[idx] = lexer->cur_char;
        lex_advance(lexer, src, src_size);
        idx++;
    }

    lexer->token_buf[idx] = '\0';
    // move the cursor forward by one if we landed on a seperator
    if(lexer->cur_char == ',')
        lex_advance(lexer, src, src_size);
}

/*
 * lex_next_token()
 */
void lex_next_token(Lexer* lexer, Token* token, const char* src, size_t src_size)
{
    // lex the next token, this places a new string into lexer->token_buf
    lex_scan_token(lexer, src, src_size);

    // Now check the token in the buffer
}




// TODO : this will be the entry point for the lexer
int lex_file(Lexer* lexer, const char* src, size_t src_size)
{
    return 0;
}
