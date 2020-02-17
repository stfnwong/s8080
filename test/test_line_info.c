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
        check(test_info->error == 0);

        // Also check that the register values are initialized correctly
        for(int r = 0; r < LINE_INFO_NUM_REG; ++r)
            check(test_info->reg[r] == '\0');

        // Opcode check
        check(test_info->opcode != NULL);
        check(test_info->opcode->instr == 0);
        // default for opcode should be a null string
        check(strlen(test_info->opcode->mnemonic) == 0);

        line_info_destroy(test_info);
    }
}
