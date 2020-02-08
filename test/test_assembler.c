/*
 * TEST_ASSEMBLER
 * Unit tests for Assembler object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "source.h"
// testing framework
#include "bdd-for-c.h"

static const char test_filename[] = "asm/test_lexer.asm";
static const char mov_test_filename[] = "asm/test_mov.asm";
static const char arith_test_filename[] = "asm/test_arith.asm";

spec("Assembler")
{
    it("Initializes correctly")
    {
        Assembler* assem;

        assem = assembler_create();
        check(assem != NULL);
        check(assem->src_repr == NULL);
        check(assem->cur_line == 0);
        check(assem->verbose == 0);

        assembler_destroy(assem);
    }


    it("Should allocate instruction memory when a SourceInfo object is attached")
    {
        Assembler* assem;
        SourceInfo* test_repr;
        int num_lines = 32;

        assem = assembler_create();
        check(assem != NULL);
        check(assem->src_repr == NULL);
        check(assem->instr_buf == NULL);
        check(assem->instr_buf_size == 0);
        check(assem->cur_line == 0);
        check(assem->verbose == 0);

        // Create a SourceInfo to assemble
        test_repr = source_info_create(num_lines);
        check(test_repr != NULL);
        check(test_repr->max_size == num_lines);

        // Create a lineinfo handle
        LineInfo* test_line = line_info_create();
        check(test_line != NULL);
        Opcode* test_opcode = opcode_create();
        check(test_opcode != NULL);

        for(int l = 0; l < test_repr->max_size; ++l)
        {
            line_info_init(test_line);
            opcode_init(test_opcode);

            test_line->addr = l+1;
            test_line->has_immediate = (l % 2 == 0) ? 1 : 0;
            test_line->immediate = (l % 2 == 0) ? l : 0;
            source_info_add_line(test_repr, test_line);
        }
        assembler_set_repr(assem, test_repr);
        check(assem->src_repr != NULL);
        check(assem->instr_buf != NULL);
        check(assem->instr_buf_size == test_repr->max_size);

        assembler_destroy(assem);
    }

    // TODO : need to bring in Lexer here
    //it("Should assemble the arithmetic instruction test file")
    //{
    //}

    //it("Assembles ADD instruction")
    //{
    //    Assembler* assem;

    //    assem = assembler_create();
    //    check(assem != NULL);

    //    Opcode* add_opcode = opcode_create();
    //    check(add_opcode != NULL);
    //    LineInfo* add_info = line_info_create();
    //    check(add_info != NULL);
    //    SourceInfo* src_repr = source_info_create(4);
    //    check(src_repr != NULL);

    //    // setup the LineInfo struct
    //    add_opcode->instr    = LEX_ADD;
    //    strcpy(add_opcode->mnemonic, "ADD\0");
    //    add_info->opcode = add_opcode;
    //    add_info->reg[0] = 'A';
    //    add_info->addr = 0xBEEF;

    //    source_info_add_line(src_repr, add_info);
    //    
    //    assembler_set_repr(assem, src_repr);
    //    assembler_assem(assem);


    //    assembler_destroy(assem);
    //    line_info_destroy(add_info);
    //}
}
