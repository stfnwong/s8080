/*
 * TEST_SOURCE_INFO
 * Unit tests for SourceInfo object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "source.h"
// testing framework
#include "bdd-for-c.h"


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

        // Are all the elements initialized?
        for(int i = 0; i < test_info->max_size; ++i)
        {
            // we expect the memory to be allocated (since 
            // elements are copied into the buffer)
            check(test_info->buffer[i] != NULL);
            // But each element with have the default/init
            // values
            check(test_info->buffer[i]->line_num == 0);
            check(test_info->buffer[i]->addr == 0);
            check(test_info->buffer[i]->immediate == 0);
            check(test_info->buffer[i]->has_immediate == 0);
            check(test_info->buffer[i]->label_str_len == 0);
            check(test_info->buffer[i]->error == 0);
            check(test_info->buffer[i]->label_str == NULL);
            for(int r = 0; r < LINE_INFO_NUM_REG; ++r)
                check(test_info->buffer[i]->reg[r] == REG_NONE);

            // Opcode is initnalized 
            check(test_info->buffer[i]->opcode->instr == 0);
            for(int r = 0; r < OPCODE_MNEMONIC_SIZE; ++r)
                check(test_info->buffer[i]->opcode->mnemonic[r] == 0);
            check(test_info->buffer[i]->byte_list != NULL);
        }

        source_info_destroy(test_info);
    }

    it("Can be cloned with source_info_clone")
    {
        int num_elems;
        SourceInfo* src_info;
        SourceInfo* dst_info;

        num_elems = 64;

        src_info = source_info_create(num_elems);
        check(src_info != NULL)
        check(src_info->max_size == num_elems);
        check(src_info->size == 0);

        LineInfo* dummy_line = line_info_create();
        check(dummy_line != NULL);
        // Generate some dummy data to place into the source info buffer
        for(int e = 0; e < src_info->size; ++e)
        {
            line_info_init(dummy_line);
            dummy_line->line_num = e+1;
            dummy_line->addr = 0xBEEF + e;
            dummy_line->has_immediate = (e % 2 == 0) ? 1 : 0;
            dummy_line->immediate = (e % 2 == 0) ? e : 0;
            source_info_add_line(src_info, dummy_line);
        } 

        // Make the clone
        dst_info = source_info_clone(src_info);
        check(dst_info->size == src_info->size);
        check(dst_info->max_size == src_info->max_size);

        // Check all elements
        for(int e = 0; e < src_info->size; ++e)
        {
            LineInfo* cur_line = dst_info->buffer[e];
            if((e % 2) == 0)
            {
                check(cur_line->has_immediate == 1);
                check(cur_line->immediate == e);
            }
            else
                check(cur_line->has_immediate == 0);
            check(cur_line->line_num == e+1);
            check(cur_line->addr == 0xBEEF + e);
            // Check that the pointers are not equal
            check(dst_info->buffer[e] != src_info->buffer[e]);
        }
    
        source_info_destroy(src_info);
        source_info_destroy(dst_info);
        line_info_destroy(dummy_line);
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
        test_line->reg[0]   = REG_A;
        test_line->reg[1]   = REG_C;
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

        out_line = source_info_get_idx(test_info, 0);
        line_info_print(out_line);

        check(out_line->line_num == test_line->line_num);
        check(out_line->addr == test_line->addr);
        for(int r = 0; r < 3; ++r)
            check(out_line->reg[r] == test_line->reg[r]);

        // calling this again should insert into next position
        test_line->addr += 4;
        test_line->reg[0] = REG_B;
        free(test_line->label_str);
        test_line->label_str = NULL;
        test_line->label_str_len = 0;

        status = source_info_add_line(test_info, test_line);
        check(status == 0);

        // Check the line we just inserted.
        check(test_info->size == 2);
        out_line = source_info_get_idx(test_info, 1);

        check(out_line->addr == test_line->addr);
        check(out_line->line_num == test_line->line_num);
        for(int r = 0; r < 3; ++r)
            check(out_line->reg[r] == test_line->reg[r]);

        source_info_destroy(test_info);
    }

    it("Should not allow insert of more than N elements")
    {
        int num_elems;
        int status;
        SourceInfo* test_info;
        LineInfo*   test_line;
        Opcode* test_op;

        num_elems = 2;

        test_info = source_info_create(num_elems);
        check(test_info->max_size == num_elems);
        check(test_info->size == 0);

        // Add some stuff 
        test_line = line_info_create();
        check(test_line != NULL);
        test_op = opcode_create();
        check(test_op != NULL);

        for(int l = 0; l < test_info->max_size; ++l)
        {
            line_info_init(test_line);
            opcode_init(test_op);

            // Make some dummy data 
            test_line->addr = l + 0xBEEF;
            test_line->line_num = l + 1;
            test_op->instr = LEX_ADD;
            strncpy(test_op->mnemonic, "ADD\0", 4);
            test_line->opcode = test_op;
            test_line->reg[0] = REG_E;
            // add the line
            status = source_info_add_line(test_info, test_line);
            check(status == 0);
        }

        // The object should now be full
        check(test_info->size == num_elems);
        check(source_info_full(test_info) == 1);
        check(source_info_empty(test_info) == 0);

        // If we add another item we should get the status code -1
        status = source_info_add_line(test_info, test_line);
        check(status == -1);

        source_info_destroy(test_info);
        line_info_destroy(test_line);
    }

    // TODO : more bounds check tests

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
