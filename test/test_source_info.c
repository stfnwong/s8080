/*
 * TEST_SOURCE_INFO
 * Unit tests for SourceInfo object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "lexer.h"
// testing framework
#include "bdd-for-c.h"


char* src_buf;
int src_file_size;


spec("SourceInfo")
{
    it("Should init with N elements, all empty")
    {
        int num_elems;
        SourceInfo* test_info;

        num_elems = 64;

        test_info = source_info_create(num_elems);
        check(test_info->max_size == num_elems);
        check(test_info->size == 0);

        source_info_destroy(test_info);
    }

    it("Allows insert")
    {
        int num_elems;
        int status;
        const char* label = "LABEL\0";
        SourceInfo* test_info;
        Opcode test_op;

        num_elems = 64;

        test_info = source_info_create(num_elems);
        check(test_info->max_size == num_elems);
        check(test_info->size == 0);

        // Create a LineInfo
        LineInfo* test_line;
        LineInfo* out_line;

        test_line = line_info_create();
        check(test_line != NULL);

        test_line->line_num = 44;
        test_line->addr     = 0xBEEF;
        test_line->reg[0]   = 'A';
        test_line->reg[1]   = 'C';
        // add a labelc
        test_line->label_str = malloc(sizeof(char) * strlen(label));
        check(test_line->label_str != NULL);
        strncpy(test_line->label_str, label, strlen(label));
        // add an opcode
        test_op.instr = LEX_MOV;
        strncpy(test_op.mnemonic, "MOV\0", 4);
        opcode_copy(test_line->opcode, &test_op);

        line_info_print(test_line);

        // Insert that LineInfo
        status = source_info_add_line(test_info, test_line);
        check(status == 0);

        // Check the line we just inserted.
        check(test_info->size == 1);
        check(test_info->cur_line == 1);

        out_line = source_info_get_idx(test_info, 0);

        line_info_print(out_line);

        fprintf(stdout, "[%s] &out_line = %p, &test_info = %p\n", 
               __func__, &out_line, &test_info
        );

        check(out_line->line_num == test_line->line_num);
        check(out_line->addr == test_line->addr);
        for(int r = 0; r < 3; ++r)
            check(out_line->reg[r] == test_line->reg[r]);

        // calling this again should insert into next position
        test_line->addr += 4;
        test_line->reg[0] = 'B';
        free(test_line->label_str);
        test_line->label_str = NULL;
        test_line->label_str_len = 0;

        status = source_info_add_line(test_info, test_line);
        check(status == 0);

        // Check the line we just inserted.
        check(test_info->size == 2);
        check(test_info->cur_line == 2);
        out_line = source_info_get_idx(test_info, 1);


        check(out_line->addr == test_line->addr);
        check(out_line->line_num == test_line->line_num);
        for(int r = 0; r < 3; ++r)
            check(out_line->reg[r] == test_line->reg[r]);

        //// We can manually insert by idx as well;
        //test_line->addr += 4;
        //status = source_info_edit_line(test_info, test_line, 6);

        //check(status == 0);

        //check(test_info->size == 3);
        //check(test_info->cur_line == 2);
        //out_line = source_info_get_idx(test_info, 6);

        //check(out_line->addr == test_line->addr);
        //check(out_line->line_num == test_line->line_num);
        //for(int r = 0; r < 3; ++r)
        //    check(out_line->reg[r] == test_line->reg[r]);


        source_info_destroy(test_info);
    }

    //it("Allows access by index")
    //{
    //    int num_elems;
    //    SourceInfo* test_info;

    //    num_elems = 64;

    //    test_info = source_info_create(num_elems);
    //    check(test_info->max_size == num_elems);
    //    check(test_info->size == 0);

    //    // Creata a new LineInfo structure and place it in the buffer

    //    source_info_destroy(test_info);
    //}
}
