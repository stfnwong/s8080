/*
 * SOURCE
 * Stuff for dealing with assembly source
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "source.h"


const char* TOKEN_TYPE_TO_STR[] = {
    "NONE",
    "LITERAL",
    "LABEL",
    "DIRECTIVE",
    "INSTR",
    "REGISTER",
    "STRING",
    "EOF"
};

const char* REG_TYPE_TO_STR[] = {
    "NONE", "A", "B", "C", "D", "H", "L", "M", "PSW"
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

    info->label_str  = NULL;
    info->symbol_str = NULL;
    info->byte_array = NULL;
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
    for(int a = 0; a < LINE_INFO_NUM_REG; ++a)
        info->reg[a] = REG_NONE;

    // Ensure that there is no string memory
    if(info->label_str != NULL)
    {
        free(info->label_str);
        info->label_str = NULL;
    }
    info->label_str_len = 0;
    if(info->symbol_str != NULL)
    {
        free(info->symbol_str);
        info->symbol_str = NULL;
    }
    info->symbol_str_len = 0;
    if(info->byte_array != NULL)
    {
        free(info->byte_array);
        info->byte_array = NULL;
    }
    info->byte_array_len = 0;
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

    if(info->label_str_len > 0 && (info->label_str != NULL))
        fprintf(stdout, "    label  : %s\n", info->label_str);

    fprintf(stdout, "    line %d : addr 0x%04X\n", info->line_num, info->addr);
    if(info->has_immediate)
        fprintf(stdout, "    imm    : %d (0x%X)\n", info->immediate, info->immediate);
    else
        fprintf(stdout, "    imm    : none\n");

    fprintf(stdout, "    error  : %s\n", (info->error) ? "YES" : "NO");
    fprintf(stdout, "    Opcode : ");
    opcode_print(info->opcode);
    if(info->symbol_str_len > 0)
        fprintf(stdout, " [%s] ", info->symbol_str);
    fprintf(stdout, "\n");
}

/*
 * line_info_print_instr()
 */
void line_info_print_instr(LineInfo* info)
{
    if(info == NULL || info->opcode == NULL)
        return;

    // NOTE : valgrind gives invalid read of size 1 here
    if(info->label_str_len > 0)
        fprintf(stdout, "%s: ", info->label_str);

    fprintf(stdout, "%s ", info->opcode->mnemonic);
    switch(info->opcode->instr)
    {
        // One register argument
        case LEX_ACI:
        case LEX_ADD:
        case LEX_ADC:
        case LEX_ANA:
        case LEX_CMP:
        case LEX_DAA:
        case LEX_INR:
        case LEX_INX:
        case LEX_LDAX:
        case LEX_ORA:
        case LEX_DAD:
        case LEX_POP:
        case LEX_PUSH:
        case LEX_SBB:
        case LEX_STAX:
        case LEX_SUB:
        case LEX_XRA:
            fprintf(stdout, "%s ", REG_TYPE_TO_STR[info->reg[0]]);
            break;

        // Two register arguments 
        case LEX_MOV:
            fprintf(stdout, "%s, %s", REG_TYPE_TO_STR[info->reg[0]], REG_TYPE_TO_STR[info->reg[1]]);
            break;

        // One register and one 8bit immediate
        case LEX_MVI:
            fprintf(stdout, "%s 0x%02X ", REG_TYPE_TO_STR[info->reg[0]], info->immediate);
            break;
            
        // One register and one 16bit immediate
        case LEX_LXI:
            fprintf(stdout, "%s 0x%04X ", REG_TYPE_TO_STR[info->reg[0]], info->immediate);
            break;

        // 8-bit immediate arguments 
        case LEX_ADI:
            fprintf(stdout, "0x%02X ", info->immediate);
            break;

        // 16-bit Immediate arguments
        case LEX_LHLD:
        case LEX_STA:
            fprintf(stdout, "%04X", info->immediate);
            break;

        // Jump instructions 
        case LEX_JC:
        case LEX_JP:
        case LEX_JMP:
        case LEX_JNC:
        case LEX_JZ:
            if(info->symbol_str_len > 0)
                fprintf(stdout, "<%s> [0x%04X] ", info->symbol_str, info->immediate);
            else
                fprintf(stdout, "[0x%04X] ", info->immediate);
            break;

        // Subroutine call instructions 
        
        // Subroutine return instructions 

        // Data instructions
        case LEX_DB:
            if(info->byte_array_len > 0)
                fprintf(stdout, "<%s> ", info->byte_array);

        // If this instruction just has an opcode then do nothing
        default:
            break;
    }
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
    for(int r = 0; r < LINE_INFO_NUM_REG; ++r)
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
    // NOTE : use realloc here?
    if(dst->label_str_len > 0)
    {
        if(dst->label_str != NULL)
            free(dst->label_str);
        dst->label_str = malloc(sizeof(char) * dst->label_str_len);
        if(!dst->label_str)
            return -1;
        strncpy(dst->label_str, src->label_str, dst->label_str_len);
    }
    // Also copy the symbol string 
    dst->symbol_str_len = src->symbol_str_len;
    if(dst->symbol_str_len > 0)
    {
        if(dst->symbol_str != NULL)
            free(dst->symbol_str);
        dst->symbol_str = malloc(sizeof(char) * dst->symbol_str_len);
        if(!dst->symbol_str)
            return -1;
        strncpy(dst->symbol_str, src->symbol_str, dst->symbol_str_len);
    }
    dst->error = src->error;

    return 0;
}

/*
 * line_info_struct_size()
 */
int line_info_struct_size(LineInfo* info)
{
    return sizeof(*info) + info->label_str_len + sizeof(*info->opcode);
}

/*
 * line_info_set_label_str()
 */
int line_info_set_label_str(LineInfo* info, char* str, int len)
{
    if(info->label_str != NULL)
        free(info->label_str);
    info->label_str = malloc(sizeof(char) * len+1);
    if(!info->label_str)
        return -1;
    info->label_str_len = len;
    strncpy(info->label_str, str, info->label_str_len);
    info->label_str[len] = '\0';

    return 0;
}

/*
 * line_info_set_symbol_str()
 */
int line_info_set_symbol_str(LineInfo* info, char* str, int len)
{
    if(info->symbol_str != NULL)
        free(info->symbol_str);
    info->symbol_str = malloc(sizeof(char) * len+1);
    if(!info->symbol_str)
        return -1;
    info->symbol_str_len = len;
    strncpy(info->symbol_str, str, info->symbol_str_len);
    info->symbol_str[len] = '\0';

    return 0;
}

/*
 * line_info_set_byte_array()
 */
int line_info_set_byte_array(LineInfo* info, uint8_t* array, int len)
{
    if(info->byte_array != NULL)
        free(info->byte_array);
    info->byte_array = malloc(sizeof(uint8_t) * len);
    if(!info->byte_array)
        return -1;
    info->byte_array_len = len;
    memcpy(info->byte_array, array, len);

    return 0;
}


/*
 * reg_char_to_code()
 */
uint8_t reg_char_to_code(char r)
{
    switch(r)
    {
        case 'A':
        case 'a':
            return REG_A;
        case 'B':
        case 'b':
            return REG_B;
        case 'C':
        case 'c':
            return REG_C;
        case 'D':
        case 'd':
            return REG_D;
        case 'E':
        case 'e':
            return REG_E;
        case 'H':
        case 'h':
            return REG_H;
        case 'L':
        case 'l':
            return REG_L;
        case 'M':
        case 'm':
            return REG_M;
        case 'S':
        case 's':
            return REG_S;
        case 'P':
        case 'p':
            return REG_PSW;
    }

    return REG_NONE;
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
    // NOTE : not sure that all the memory is getting cleared here...
    if(info == NULL)
        free(info);
    else if(info->size == 0)
    {
        //free(info->buffer);     // fails?
        free(info);
    }
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

    // Bounds check the insert
    if(info->size == info->max_size)
        return -1;
      
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


/*
 * source_info_clone()
 */
SourceInfo* source_info_clone(SourceInfo* src)
{
    SourceInfo* dst;

    dst = source_info_create(src->max_size);
    if(!dst)
        goto CLONE_END;

    dst->size     = src->size;
    dst->cur_line = src->cur_line;
    dst->max_size = src->max_size;

    for(int e = 0; e < src->size; ++e)
        dst->buffer[e] = src->buffer[e];

CLONE_END:
    if(!dst)
    {
        fprintf(stdout, "[%s] failed to allocate memory for clone\n", __func__);
        return NULL;
    }

    return dst;
}

/*
 * source_info_full()
 */
int source_info_full(SourceInfo* info)
{
    return (info->size == info->max_size) ? 1 : 0;
}

/*
 * source_info_empty()
 */
int source_info_empty(SourceInfo* info)
{
    return (info->size == 0) ? 1 : 0;
}

/*
 * source_info_write()
 */
int source_info_write(SourceInfo* info, const char* filename)
{
    FILE* fp;

    fp = fopen(filename, "wb");
    if(!fp)
    {
        fprintf(stderr, "[%s] failed to open file %s for writing\n",
               __func__, filename);
        return -1;
    }

    // Write the number of records, and the max size
    fwrite(&info->size, sizeof(int), 1, fp);        
    fwrite(&info->max_size, sizeof(int), 1, fp);        

    // Now write each of the Lineinfo structures


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
    token->type          = SYM_NONE;
    token->token_str_len = 0;
    token->token_str[0]  = '\0';     // or memset()?

    return token;
}

/*
 * token_init()
 */
void token_init(Token* token)
{
    token->type = SYM_NONE;
    token->token_str_len = 0;
    memset(token->token_str, 0, TOKEN_BUF_SIZE);
    //token->token_str[0] = '\0';
}

/*
 * destroy_token()
 */
void destroy_token(Token* token)
{
    free(token);
}
