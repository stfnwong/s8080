/*
 * TEST_LINE_INFO
 * Unit tests for LineInfo object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "source.h"
// testing framework
#include "bdd-for-c.h"

spec("LineInfo")
{
    it("Should init correctly")
    {
        LineInfo* test_info;

        test_info = line_info_create();
        check(test_info != NULL);
        check(test_info->line_num == 0);
        check(test_info->addr == 0);
        check(test_info->has_immediate == 0);
        check(test_info->immediate == 0);
        check(test_info->label_str == NULL);
        check(test_info->label_str_len == 0);
        check(test_info->symbol_str == NULL);
        check(test_info->symbol_str_len == 0);
        check(test_info->byte_list != NULL);
        check(test_info->error == 0);

        // Also check that the register values are initialized correctly
        for(int r = 0; r < LINE_INFO_NUM_REG; ++r)
            check(test_info->reg[r] == REG_NONE);

        // Opcode check
        check(test_info->opcode != NULL);
        check(test_info->opcode->instr == 0);
        // default for opcode should be a null string
        check(strlen(test_info->opcode->mnemonic) == 0);

        line_info_destroy(test_info);
    }

    it("Should copy correctly")
    {
        LineInfo* src_info;
        LineInfo* dst_info;
        int status;

        src_info = line_info_create();
        check(src_info != NULL);
        dst_info = line_info_create();
        check(dst_info != NULL);

        // Put some stuff in the source info
        src_info->line_num = 55;
        src_info->addr = 0xBEEF;
        src_info->immediate = 36;
        src_info->has_immediate = 1;
        // Need to allocate the source memory here ...
        status = line_info_set_symbol_str(src_info, "TEST_SYMBOL\0", strlen("TEST_SYMBOL\0"));
        check(status == 0);
        src_info->symbol_str_len = strlen("TEST_SYMBOL\0");
        status = line_info_set_label_str(src_info, "TEST_LABEL\0", strlen("TEST_LABEL\0"));
        check(status == 0);

        src_info->reg[0] = REG_A;
        src_info->reg[1] = REG_H;

        status = line_info_copy(dst_info, src_info);
        check(status == 0);

        // Compare the fields
        check(src_info->line_num == dst_info->line_num);
        check(src_info->addr == dst_info->addr);
        check(src_info->immediate == dst_info->immediate);
        check(src_info->has_immediate == dst_info->has_immediate);
        check(src_info->label_str_len == dst_info->label_str_len);
        check(src_info->symbol_str_len == dst_info->symbol_str_len);
        check(strncmp(src_info->label_str, dst_info->label_str, src_info->label_str_len) == 0);
        check(strncmp(src_info->symbol_str, dst_info->symbol_str, src_info->symbol_str_len) == 0);

        line_info_destroy(src_info);
        line_info_destroy(dst_info);
    }

    /*
     * Note that the motivation for this requirement is due to static 
     * data being 'inline' in 8080 assembly. In other words, there isn't
     * a data segment. 
     */
    it("Should allow inline data to be appended")
    {
        int status;
        LineInfo* test_info;

        test_info = line_info_create();
        check(test_info != NULL);
        check(test_info->line_num == 0);
        check(test_info->addr == 0);

        // Lets set up a DB instruction
        //check(test_info->opcode->instr == LEX_DB);
        test_info->opcode->instr = LEX_DB;
        strncpy(test_info->opcode->mnemonic, "DB\0", 3);

        int data_size = 32;
        uint8_t* test_data = malloc(sizeof(uint8_t) * data_size);
        uint16_t test_addr = 0xDEED;
        check(test_data != NULL);

        for(int i = 0; i < data_size; ++i)
            test_data[i] = (i+1) % 256;

        // Append some test data to the line. Imagine that this 
        // is a string of bytes that is an argument to DB
        check(line_info_num_bytes(test_info) == 0);
        status = line_info_append_byte_array(test_info, test_data, data_size, test_addr);
        check(status == 0);
        check(line_info_num_bytes(test_info) == 1);

        byte_list_print(test_info->byte_list);
        fprintf(stdout, "\n");

        // We can add another single byte
        status = line_info_append_byte_array(test_info, test_data, 1, test_addr+1);
        check(status == 0);
        check(line_info_num_bytes(test_info) == 2);

        byte_list_print(test_info->byte_list);
        fprintf(stdout, "\n");

        // The sizes of each of the segments does not need to be equal
        for(int i = 0; i < data_size; ++i)
            test_data[i] = (i+2) % 256;

        status = line_info_append_byte_array(test_info, test_data, data_size >> 1, test_addr + data_size + 1);
        check(status == 0);
        check(line_info_num_bytes(test_info) == 3);

        byte_list_print(test_info->byte_list);
        fprintf(stdout, "\n");

        // If we clear the list now the size will reduce to zero
        line_info_delete_bytes(test_info);
        check(line_info_num_bytes(test_info) == 0);
        check(byte_list_total_bytes(test_info->byte_list) == 0);

        byte_list_print(test_info->byte_list);
        fprintf(stdout, "\n");

        line_info_destroy(test_info);
        free(test_data);
    }
}
