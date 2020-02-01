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

const char* TOKEN_TYPE_TO_STR[] = {
    "NONE",
    "LITERAL",
    "LABEL",
    "INSTR",
    "REGISTER",
    "EOF"
};

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

    info->label_str = NULL;
    line_info_init(info);

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
    free(info->label_str);
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
    info->has_immediate = 0;
    info->immediate = 0;
    for(int a = 0; a < 3; ++a)
        info->reg[a] = '\0';

    // Ensure that there is no string memory
    if(info->label_str != NULL)
    {
        free(info->label_str);
        info->label_str = NULL;
    }
    info->label_str_len = 0;
    info->error = 0;
}

/*
 * line_info_print()
 */
// Would actually be better to format into a string, but this is C and 
// that's a major pain 
void line_info_print(LineInfo* info)
{
    //fprintf(stdout, "LineInfo (line %d)\n", info->line_num);
    fprintf(stdout, "LineInfo :\n");

    fprintf(stdout, "    line %d : addr 0x%04X\n", info->line_num, info->addr);
    if(info->has_immediate)
        fprintf(stdout, "    imm    : %d (0x%X)\n", info->immediate, info->immediate);
    else
        fprintf(stdout, "    no immediate\n");

    if(info->label_str_len > 0 && (info->label_str != NULL))
        fprintf(stdout, "    label  : %s\n", info->label_str);

    fprintf(stdout, "    error  : %s\n", (info->error) ? "YES" : "NO");
    fprintf(stdout, "    Opcode : ");
    opcode_print(info->opcode);
    fprintf(stdout, "\n");
}

/*
 * line_info_copy()
 */
int line_info_copy(LineInfo* dst, LineInfo* src)
{
    if(src == NULL || dst == NULL)
        return -1;

    dst->line_num      = src->line_num;
    dst->addr          = src->addr;
    dst->has_immediate = src->has_immediate;
    dst->immediate     = src->immediate;
    for(int r = 0; r < 3; ++r)
        dst->reg[r] = src->reg[r];

    // copy pointers
    if(dst->opcode == NULL)
    {
        dst->opcode = opcode_create();
        if(!dst->opcode)
            return -1;
    }

    opcode_copy(dst->opcode, src->opcode);
    dst->label_str_len = src->label_str_len;
    // we may need to allocate some memory here for label string
    if(dst->label_str_len > 0)
    {
        if(dst->label_str != NULL)
            free(dst->label_str);
        dst->label_str = malloc(sizeof(char) * dst->label_str_len);
        if(!dst->label_str)
            return -1;
        strncpy(dst->label_str, src->label_str, dst->label_str_len);
    }

    dst->error = src->error;

    return 0;
}


// ================ SOURCE INFO ================ //
/*
 * source_info_create()
 */
SourceInfo* source_info_create(int num_lines)
{
    SourceInfo* info;

    info = malloc(sizeof(*info));
    if(!info)
        goto SOURCE_INFO_END;

    info->max_size = num_lines;
    info->size     = 0;
    info->cur_line = 0;
    info->buffer   = malloc(sizeof(*info->buffer) * info->max_size);
    if(!info->buffer)
        goto SOURCE_INFO_END;

    for(int b = 0; b < info->max_size; ++b)
    {
        // Seem to be leaking these when we clean up SourceInfo
        info->buffer[b] = malloc(sizeof(*info->buffer[b]));
        if(!info->buffer[b])
        {
            free(info->buffer);
            free(info);
            return NULL;
        }
        info->buffer[b] = line_info_create();
    }

SOURCE_INFO_END:
    if(!info || !info->buffer)
    {
        fprintf(stderr, "[%s] failed to allocate memory for SourceInfo\n", __func__);
        return NULL;
    }

    return info;
}

/*
 * source_info_destroy()
 */
void source_info_destroy(SourceInfo* info)
{
    if(info == NULL)
        free(info);
    else
    {
        for(int b = 0; b < info->max_size; ++b)
            line_info_destroy(info->buffer[b]);
        free(info->buffer);
        free(info);
    }
}

/*
 * source_info_add_line()
 */
int source_info_add_line(SourceInfo* info, LineInfo* line)
{
    int status; 
      
    status = line_info_copy(info->buffer[info->size], line);
    if(status >= 0)
    {
        info->size++;
        info->cur_line++;
    }

    return status;
}

/*
 * source_info_edit_line()
 */
int source_info_edit_line(SourceInfo* info, LineInfo* line, int idx)
{
    int status = 0;

    if(idx < 0 || idx > info->size)
        return -1;

    status = line_info_copy(info->buffer[idx], line);

    if(status >= 0)
    {
        info->size++;
    }

    return status;
}

/*
 * source_info_get_idx()
 */
LineInfo* source_info_get_idx(SourceInfo* info, int idx)
{
    if(idx < 0 || idx > info->max_size)
        return NULL;

    return (LineInfo*) info->buffer[idx];
}

// ================ DATA SEGMENT ================ //
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
 * token_init()
 */
void token_init(Token* token)
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

    // Don't allocate the source info object here, rather we
    // create it once we've read the source in and can guess at the 
    // number of lines we will need
    lexer->source_repr = NULL;

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
    source_info_destroy(lexer->source_repr);
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

    if(lexer->source_repr != NULL)
        source_info_destroy(lexer->source_repr);
    lexer->source_repr = source_info_create(num_lines);
    if(!lexer->source_repr)
    {
        fprintf(stderr, "[%s] failed to create new SourceInfo for lexer with %d entries\n", __func__, num_lines);
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

    //tok_ptr = lexer->cur_char;
    tok_ptr = 0;
    while(isdigit(lexer->token_buf[tok_ptr]))
        tok_ptr++;

    if(tok_ptr == 0)        // we didn't move
    {
        token->type = SYM_NONE;
        return 0;
    }

    token->type = SYM_LITERAL;
    strncpy(token->token_str, lexer->token_buf, tok_ptr);

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
 * lex_next_token()
 */
void lex_next_token(Lexer* lexer, Token* token)
{
    //int error = 0;
    Opcode opcode;
    // lex the next token, this places a new string into lexer->token_buf
    lex_scan_token(lexer);

    token_init(token);
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
 * lex_parse_one_reg()
 */
int lex_parse_one_reg(Lexer* lexer, Token* token)
{
    if(token->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[token->type]
            );
        }
        return -1;
    }
    lexer->text_seg->reg[0] = token->token_str[0];

    return 0;
}

/*
 * lex_parse_two_reg()
 */
int lex_parse_two_reg(Lexer* lexer, Token* tok_a, Token* tok_b)
{
    if(tok_a->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[tok_a->type]
            );
        }
        return -1;
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
        return -1;
    }

    lexer->text_seg->reg[0] = tok_a->token_str[0];
    lexer->text_seg->reg[1] = tok_b->token_str[0];

    return 0;
}

/*
 * lex_parse_reg_imm()
 */
int lex_parse_reg_imm(Lexer* lexer, Token* tok_a, Token* tok_b)
{
    if(tok_a->type != SYM_REG)
    {
        if(lexer->verbose)
        {
            fprintf(stdout, "[%s] line %d:%d expected register, got %s\n",
                   __func__, lexer->cur_line, 
                   lexer->cur_col, TOKEN_TYPE_TO_STR[tok_a->type]
            );
        }
        return -1;
    }

    if(tok_b->type != SYM_LITERAL)
    {
        fprintf(stdout, "[%s] line %d:%d, ERROR: expected immediate, got %s\n",
               __func__, lexer->cur_line, lexer->cur_col, 
               TOKEN_TYPE_TO_STR[tok_b->type]
        );
        return -1;
    }

    lexer->text_seg->reg[0]        = tok_a->token_str[0];
    fprintf(stdout, "[%s] getting literal.....\n", __func__);
    lexer->text_seg->immediate     = lex_extract_literal(lexer, tok_b);
    lexer->text_seg->has_immediate = 1;

    fprintf(stdout, "[%s] literal was %d.....\n", __func__, lexer->text_seg->immediate);

    return 0;
}

/*
 * lex_line()
 */
void lex_line(Lexer* lexer)
{
    int status = 0;
    Opcode cur_opcode;
    Token cur_token;
    Token tok_a, tok_b;

    token_init(&tok_a);
    token_init(&tok_b);
    line_info_init(lexer->text_seg);
    lex_next_token(lexer, &cur_token);

    if(cur_token.type == SYM_LABEL)
    {
        lexer->text_seg->label_str = malloc(sizeof(char) * strlen(cur_token.token_str));
        if(!lexer->text_seg)
        {
            fprintf(stderr, "[%s] failed to allocate memory for lexer->text_seg->label_str (%ld chars)\n", __func__, strlen(cur_token.token_str));
            status = -1;
            goto LEX_LINE_END;
        }
        // Copy label string
        strncpy(lexer->text_seg->label_str, cur_token.token_str, strlen(cur_token.token_str));
        lexer->text_seg->label_str_len = strlen(cur_token.token_str);
        // Get the next token ready
        lex_next_token(lexer, &cur_token);
    }

    if(cur_token.type == SYM_INSTR)
    {
        opcode_init(&cur_opcode);
        opcode_table_find_mnemonic(lexer->op_table, &cur_opcode, cur_token.token_str);

        switch(cur_opcode.instr)
        {
            case LEX_DCR:
                fprintf(stdout, "[%s] got DCR\n", __func__);
                break;

            case LEX_INR:
                // Increment register
                lex_next_token(lexer, &cur_token);
                status = lex_parse_one_reg(lexer, &cur_token);
                break;

            case LEX_MOV:
                fprintf(stdout, "[%s] got MOV\n", __func__);
                // For MOV, we need two registers
                lex_next_token(lexer, &tok_a);
                lex_next_token(lexer, &tok_b);

                status = lex_parse_two_reg(lexer, &tok_a, &tok_b);
                break;

            case LEX_MVI:
                fprintf(stdout, "[%s] got MVI\n", __func__);
                lex_next_token(lexer, &tok_a);
                lex_next_token(lexer, &tok_b);

                status = lex_parse_reg_imm(lexer, &tok_a, &tok_b);
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

    //if(cur_token.type == SYM_LITERAL)
    //{
    //    lexer->text_seg->literal = lex_extract_literal(lexer, &cur_token);
    //    lexer->text_seg->has_immediate = 1;
    //}


LEX_LINE_END:
    if(status < 0)
    {
        fprintf(stdout, "[%s] something went wrong...\n", __func__);
        lexer->text_seg->error = 1;
    }
    lex_text_addr_incr(lexer);

    // Need to copy text_seg to some buffer, then reset
    source_info_add_line(lexer->source_repr, lexer->text_seg);
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
    token_init(cur_token);

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
