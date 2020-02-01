/*
 * TEST_OPCODE
 * Unit tests for opcode objects
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "opcode.h"
// testing framework
#include "bdd-for-c.h"


spec("Opcode")
{

    it("Should init lexer instruction codes in OpcodeTable")
    {
        OpcodeTable* optable;

        optable = opcode_table_create();       // At time of writing there are 4 instructions
        check(optable != NULL);
        check(optable->num_opcodes == NUM_LEX_INSTR);

        fprintf(stdout, "[%s] opcode table:\n\n", __func__);
        opcode_print_table(optable);

        opcode_table_destroy(optable);
    }

    it("Should allow opcode lookup by instr")
    {
        Opcode test_op;
        OpcodeTable* optable;

        // Do the basic checks
        optable = opcode_table_create();       // At time of writing there are 4 instructions
        check(optable != NULL);
        check(optable->num_opcodes == NUM_LEX_INSTR);

        // Lets pick one that works
        opcode_table_find_instr(optable, &test_op, LEX_DCR);
        check(test_op.instr == LEX_DCR);
        check(strncmp(test_op.mnemonic, "DCR", 3) == 0);

        // Now pick one that doesn't
        opcode_table_find_instr(optable, &test_op, -1);
        check(test_op.instr == LEX_INVALID);
        check(strncmp(test_op.mnemonic, "\0\0\0", 3) == 0);

        opcode_table_destroy(optable);
    }

    it("Should allow opcode lookup by mnemonic")
    {
        Opcode test_op;
        OpcodeTable* optable;

        // Do the basic checks
        optable = opcode_table_create();       // At time of writing there are 4 instructions
        check(optable != NULL);
        check(optable->num_opcodes == NUM_LEX_INSTR);

        // Lets pick one that works
        opcode_table_find_mnemonic(optable, &test_op, "DCR");
        check(test_op.instr == LEX_DCR);
        check(strncmp(test_op.mnemonic, "DCR", 3) == 0);

        opcode_table_find_mnemonic(optable, &test_op, "INVALID");
        check(test_op.instr == LEX_INVALID);
        check(strncmp(test_op.mnemonic, "\0\0\0", 3) == 0);

        opcode_table_destroy(optable);
    }

}
