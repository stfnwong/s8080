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
    //opcode_init(info->opcode);      // TODO : name change to opcode_init

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
    opcode_init(info->opcode);
    info->line_num = 0;
    info->addr     = 0;
    
    // arguments 
    info->has_literal = 0;
    info->literal = 0;
    for(int a = 0; a < 3; ++a)
        info->reg[a] = '\0';

    // Ensure that there is no string memory
    if(info->label_str != NULL)
    {
        free(info->label_str);
        info->label_str = NULL;
    }
    info->label_str_len = 0;
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
Lexer* lexer_create(void)
{
    Lexer* lexer;

    lexer = malloc(sizeof(*lexer));
    if(!lexer)
        goto LEXER_END;

    // src params 
    lexer->src           = NULL;
    lexer->src_len       = 0;

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

    lexer->op_table = opcode_table_create();
    if(!lexer->op_table)
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
 * lex_text_addr_incr()
 */
void lex_text_addr_incr(Lexer* lexer)
{
    lexer->text_addr += 4;
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

    tok_ptr = lexer->cur_char;
    while(isdigit(lexer->token_buf[tok_ptr]))
        tok_ptr++;

    if(tok_ptr == lexer->cur_char)      // we didn't move
    {
        token->type = SYM_NONE;
        return 0;
    }

    token->type = SYM_LITERAL;
    strncpy(token->token_str, &lexer->src[lexer->cur_pos], tok_ptr);
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
void lex_next_token(Lexer* lexer, Token* token)
{
    //int error = 0;
    Opcode opcode;
    // lex the next token, this places a new string into lexer->token_buf
    lex_scan_token(lexer);

    init_token(token);
    // TODO : later on allow for strings, chars

    // Now check the token in the buffer
    if(isdigit(lexer->token_buf[0]))
    {
        token->type = SYM_LITERAL;
        goto TOKEN_END;
    }

    // We would check here for directives


    // Check for registers 
    if((strncmp(lexer->token_buf, "A", 1) == 0) || 
       (strncmp(lexer->token_buf, "B", 1) == 0) || 
       (strncmp(lexer->token_buf, "C", 1) == 0) || 
       (strncmp(lexer->token_buf, "D", 1) == 0) || 
       (strncmp(lexer->token_buf, "E", 1) == 0))
    {
        token->type = SYM_REG;
        goto TOKEN_END;
    }

    // Check if the token is an instruction
    opcode_init(&opcode);
    opcode_table_find_mnemonic(
            lexer->op_table, 
            &opcode, 
            lexer->token_buf
    );

    if(lexer->verbose)
    {
        fprintf(stdout, "[%s] got opcode %d [%s]\n", __func__, opcode.instr, INSTR_CODE_TO_STR[opcode.instr]);
    }

    if(opcode.instr != LEX_INVALID)
    {
        token->type = SYM_INSTR;
        goto TOKEN_END;
    }

    // Since we cant match anything, we treat as a label
    // NOTE: I have considered that this is not the fastest way to do
    // this, since labels come first and we first have to fail through
    // all the other possibilities before declaring a label. 

    // Must be a label
    token->type = SYM_LABEL;

TOKEN_END:
    strcpy(token->token_str, lexer->token_buf);
    if(lexer->verbose)
    {
        fprintf(stdout, "[%s]  (line %d:%d) got token [%s] of type %s with value [%s]\n",
               __func__, lexer->cur_line, lexer->cur_col, 
               lexer->token_buf, TOKEN_TYPE_TO_STR[token->type],  token->token_str
        );
    }
}


/*
 * lex_line()
 */
void lex_line(Lexer* lexer)
{
    Opcode cur_opcode;
    Token cur_token;

    lex_next_token(lexer, &cur_token);

    if(cur_token.type == SYM_INSTR)
    {
        opcode_init(&cur_opcode);
        opcode_table_find_mnemonic(lexer->op_table, &cur_opcode, cur_token.token_str);

        switch(cur_opcode.instr)
        {
            case LEX_DCR:
                fprintf(stdout, "[%s] got DCR\n", __func__);
                break;

            case LEX_MOV:
                // TODO : create two tokens on stack and pass to function that checks types, etc
                // For MOV, we need two registers
                lex_next_token(lexer, &cur_token);

                if(cur_token.type != SYM_REG)
                {
                    fprintf(stdout, "[%s] ERROR: expected argument 1 of MOV to be register, got %s\n",
                            __func__, TOKEN_TYPE_TO_STR[cur_token.type]);
                    return;
                }

                lex_next_token(lexer, &cur_token);
                if(cur_token.type != SYM_REG)
                {
                    fprintf(stdout, "[%s] ERROR: expected argument 2 of MOV to be register, got %s\n",
                            __func__, TOKEN_TYPE_TO_STR[cur_token.type]);
                    return;
                }
                break;

            case LEX_MVI:
                // Expecting two args 
                lex_next_token(lexer, &cur_token);

                // For MVI, we need a register and an immediate 

                lex_next_token(lexer, &cur_token);
                break;

            default:
                if(lexer->verbose)
                {
                    fprintf(stderr, "[%s] invalid opcode with value %02X\n", __func__, cur_opcode.instr);
                }
                break;
        }
    }

    if(cur_token.type == SYM_LITERAL)
    {
        lexer->text_seg->literal = lex_extract_literal(lexer, &cur_token);
        lexer->text_seg->has_literal = 1;
    }

    if(cur_token.type == SYM_LABEL)
    {
        // TODO : implement table of labels and addresses
        fprintf(stdout, "[%s] got label %s\n", __func__, cur_token.token_str);
    }


    lex_text_addr_incr(lexer);
}





// TODO : this will be the entry point for the lexer
int lex_all(Lexer* lexer)
{
    Token* cur_token = create_token();
    if(!cur_token)
    {
        fprintf(stdout, "[%s] failed to allocate memory for cur_token\n", __func__);
        return -1;
    }
    init_token(cur_token);

    while(lexer->cur_pos < lexer->src_len)
    {
        // eat whitespace
        if(lex_is_whitespace(lexer->cur_char))
        {
            lex_advance(lexer);
            continue;
        }

        // eat comments
        if(lex_is_comment(lexer->cur_char))
        {
            lex_skip_comment(lexer);
            continue;
        }

        // This is a valid line, so start trying to get tokens together
        lex_line(lexer);
    }

    // TODO : labels, label resolution

    return 0;
}
