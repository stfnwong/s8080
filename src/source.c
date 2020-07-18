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
#include "vector.h"


const char* TOKEN_TYPE_TO_STR[] = {
    "NONE",
    "LITERAL",
    "LABEL",
    "DIRECTIVE",
    "INSTR",
    "REGISTER",
    "STRING",
    "COMMA",
    "EOF"
};

const char* REG_TYPE_TO_STR[] = {
    "NONE", "A", "B", "C", "D", "E", "H", "L", "S", "M", "PSW"
};

// ================ LINE INFO ================ //
LineInfo* line_info_create(void)
{
    LineInfo* info;

    info = malloc(sizeof(*info));
    if(!info)
        goto LINE_INFO_CREATE_END;

    info->opcode = malloc(sizeof(*info->opcode));
    if(!info->opcode)
        goto LINE_INFO_CREATE_END;

    info->label_str[0]  = '\0';
    info->symbol_str[0] = '\0';
    line_info_init(info);

    // Init byte list 
    info->byte_list = byte_list_create();

LINE_INFO_CREATE_END:
    if(!info || !info->opcode || !info->byte_list)
    {
        fprintf(stderr, "[%s] failed to allocate memory while creating LineInfo\n", __func__);
        free(info->opcode);
        free(info->byte_list);

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
    info->has_immediate = 0;
    info->immediate = 0;
    for(int a = 0; a < LINE_INFO_NUM_REG; ++a)
        info->reg[a] = REG_NONE;

    // Reset all the strings
    if(info->label_str[0] != '\0')  // same as strlen(info->label_str) == 0?
        info->label_str[0] = '\0';
    info->label_str_len = 0;
    if(info->symbol_str[0] != '\0')
        info->symbol_str[0] = '\0';
    info->symbol_str_len = 0;
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

    if((info->label_str_len > 0) && (info->label_str != NULL))
        fprintf(stdout, "    label  : %s\n", info->label_str);

    fprintf(stdout, "    line %d : addr 0x%04X\n", info->line_num, info->addr);
    if(info->has_immediate)
        fprintf(stdout, "    imm    : %d (0x%X)\n", info->immediate, info->immediate);
    else
        fprintf(stdout, "    imm    : none\n");

    fprintf(stdout, "    error  : %s\n", (info->error) ? "YES" : "NO");
    fprintf(stdout, "    Opcode : ");
    opcode_print(info->opcode);  // <- TODO : memory issues with this call...
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
            fprintf(stdout, "%s #$%02X ", REG_TYPE_TO_STR[info->reg[0]], info->immediate);
            break;
            
        // One register and one 16bit immediate
        case LEX_LXI:
            fprintf(stdout, "%s #$%04X ", REG_TYPE_TO_STR[info->reg[0]], info->immediate);
            break;

        // 8-bit immediate arguments 
        case LEX_ADI:
            fprintf(stdout, "#$%02X ", info->immediate);
            break;

        // 16-bit Immediate arguments
        case LEX_LHLD:
        case LEX_STA:
            fprintf(stdout, "#$%04X", info->immediate);
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
        // NOTE: how will these work now that the data is in a new place

        // System instructions 
        //case LEX_RST:
        //    fprintf(stdout, "%d ", info->immediate);
        //    break;

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

    if(dst->label_str_len > 0)
        strncpy(dst->label_str, src->label_str, dst->label_str_len);

    // Also copy the symbol string 
    dst->symbol_str_len = src->symbol_str_len;
    if(dst->symbol_str_len > 0)
        strncpy(dst->symbol_str, src->symbol_str, dst->symbol_str_len);

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
    if(len < SOURCE_INFO_MAX_LABEL_LEN)
        info->label_str_len = len;
    else
        info->label_str_len = SOURCE_INFO_MAX_LABEL_LEN - 1;

    strncpy(info->label_str, str, info->label_str_len);

    return 0;
}

/*
 * line_info_set_symbol_str()
 */
int line_info_set_symbol_str(LineInfo* info, char* str, int len)
{
    if(len < SOURCE_INFO_MAX_SYMBOL_LEN)
        info->symbol_str_len = len;
    else
        info->symbol_str_len = SOURCE_INFO_MAX_SYMBOL_LEN;

    strncpy(info->symbol_str, str, info->symbol_str_len);
    info->symbol_str[len] = '\0';

    return 0;
}

/*
 * line_info_delete_bytes()
 */
void line_info_delete_bytes(LineInfo* info)
{
    byte_list_init(info->byte_list);
}

/*
 * line_info_num_bytes()
 */
int line_info_num_bytes(LineInfo* info)
{
    return byte_list_total_bytes(info->byte_list);
}

/*
 * line_info_append_byte_array()
 */
int line_info_append_byte_array(LineInfo* info, uint8_t* data, int len, uint16_t addr)
{
    ByteNode* new_node;

    new_node = byte_node_create(data, len, addr);
    if(!new_node)
        return -1;
    return byte_list_append_node(info->byte_list, new_node);
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
SourceInfo* source_info_create(int start_capacity)
{
    SourceInfo* info;

    info = malloc(sizeof(*info));
    if(!info)
        goto SOURCE_INFO_END;

    info->capacity = start_capacity;
    info->size     = 0;
    info->buffer   = malloc(sizeof(*info->buffer) * info->capacity);
    if(!info->buffer)
        goto SOURCE_INFO_END;

    // TODO : I think that not only do we have to do this here, but also 
    // we need to do this in the code to extend the vector
    for(int b = 0; b < info->capacity; ++b)
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

    // Allocate memory for byte list
    info->byte_list = byte_list_create();
    if(!info->byte_list)
        goto SOURCE_INFO_END;


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
    for(int l = 0; l < info->size; ++l)
        line_info_destroy(info->buffer[l]);

    byte_list_destroy(info->byte_list);
    free(info);

    //if(info == NULL)
    //    free(info);
    //else if(info->size == 0)
    //{
    //    //free(info->buffer);     // fails?
    //    free(info);
    //}
    //else
    //{
    //    for(int b = 0; b < info->capacity; ++b)
    //        line_info_destroy(info->buffer[b]);
    //    free(info->buffer);
    //    free(info);
    //}
}

/*
 * source_info_add_line()
 */
void source_info_add_line(SourceInfo* info, LineInfo* line)
{
    int status; 

    // TODO: extend needs to alloc
    if(info->size >= info->capacity)
        source_info_extend(info, info->capacity);

    //memcpy(info->buffer + info->size, line, sizeof(*info->buffer));
    
    info->size++;

    // Bounds check the insert
    //if(info->size == info->capacity)
    //    return -1;
    //  
    //status = line_info_copy(info->buffer[info->size], line);
    //if(status >= 0)
    //    info->size++;

    //return status;
}

/*
 * source_info_extend()
 */
void source_info_extend(SourceInfo* info, int ext_size)
{
    LineInfo** buf;

    //buf = malloc(sizeof(*buf) * info->capacity * ext_size);
    // TODO : actually not sure if sizeof(*buf->[0] is correct... since this is the size of 
    // a single LineInfo pointer (and not the size of a LineInfo)
    
    buf = realloc(info->buffer, sizeof(*buf[0]) * ext_size);  // this copies the previous data over 
    if(!buf)
    {
        fprintf(stdout, "[%s] failed to alloc %d bytes to extend vector\n", __func__, info->capacity * ext_size);
        return;
    }

    info->capacity = info->capacity + ext_size;
    memcpy(buf, info->buffer, sizeof(*buf) * info->size);
    free(info->buffer);
    info->buffer = buf;
}

/*
 * source_info_edit_line()
 */
int source_info_edit_line(SourceInfo* info, LineInfo* line, int idx)
{
    int status = 0;

    if(idx < 0 || idx > info->size)
        return -1;

    // TODO : how does the copy work in the vector implementation?
    status = line_info_copy(info->buffer[idx], line);
    if(status >= 0)
        info->size++;

    return status;
}

/*
 * source_info_get_idx()
 */
LineInfo* source_info_get_idx(SourceInfo* info, int idx)
{
    if(idx < 0 || idx > info->capacity)
        return NULL;

    return (LineInfo*) info->buffer[idx];
}

/*
 * source_info_clone()
 */
SourceInfo* source_info_clone(SourceInfo* src)
{
    SourceInfo* dst;

    dst = source_info_create(src->capacity);
    if(!dst)
        goto CLONE_END;

    dst->size     = src->size;
    dst->capacity = src->capacity;

    // TODO : update this to reflec the new pointer structure
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
 * source_info_empty()
 */
int source_info_empty(SourceInfo* info)
{
    return (info->size == 0) ? 1 : 0;
}

/*
 * source_info_size()
 */
int source_info_size(SourceInfo* info)
{
    return info->size;
}

/*
 * source_info_capacity()
 */
int source_info_capacity(SourceInfo* info)
{
    return info->capacity;
}

/*
 * source_info_byte_list_size()
 */
int source_info_byte_list_size(SourceInfo* info)
{
    return info->byte_list->len;
}

/*
 * source_info_byte_list_num_bytes()
 */
int source_info_byte_list_num_bytes(SourceInfo* info)
{
    return byte_list_total_bytes(info->byte_list);
}

/*
 * line_info_clear_byte_list()
 */
// Probably some optimizations can be done here later
void source_info_clear_byte_list(SourceInfo* info)
{
    byte_list_destroy(info->byte_list);
    info->byte_list = byte_list_create();
}

/*
 * source_info_append_byte_array()
 */
int source_info_append_byte_array(SourceInfo* info, uint8_t* array, int len, int start_addr)
{
    int status;

    status = byte_list_append_data(
            info->byte_list,
            array,
            len,
            start_addr
    );

    return status;
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
