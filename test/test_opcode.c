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
        Opcode* op;

        op = opcode_create();
        check(op != NULL);
        optable = opcode_table_create();       
        check(optable != NULL);
        check(optable->num_opcodes == NUM_LEX_INSTR);
        fprintf(stdout, "[%s] placed %d instructions into opcode table\n", __func__, NUM_LEX_INSTR);

        fprintf(stdout, "[%s] opcode table:\n\n", __func__);
        opcode_table_print(optable);
        fprintf(stdout, "[%s] NUM_LEX_INSTR = %d\n", __func__, NUM_LEX_INSTR);

        // TODO : move to lookup by instr section
        // Check that all the instructions are actually in the table
        fprintf(stdout, "[%s] checking instructions in table\n", __func__);
        for(int i = 0; i < NUM_LEX_INSTR; ++i)
        {
            opcode_table_find_instr(
                    optable,
                    op,
                    LEX_INSTRUCTIONS[i].instr
                    );
            fprintf(stdout, "[%d / %d] ", i+1, NUM_LEX_INSTR);
            opcode_print(op);
            fprintf(stdout, "\n");
            check(op != NULL);
            check(op->instr == LEX_INSTRUCTIONS[i].instr);
            check(strcmp(op->mnemonic, LEX_INSTRUCTIONS[i].mnemonic) == 0);
        }

        opcode_table_destroy(optable);
        opcode_destroy(op);
    }

    it("Should allow opcode lookup by instr")
    {
        Opcode test_op;
        OpcodeTable* optable;

        // Do the basic checks
        optable = opcode_table_create();       
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
        optable = opcode_table_create();       
        check(optable != NULL);
        check(optable->num_opcodes == NUM_LEX_INSTR);

        // Lets pick one that works
        opcode_table_find_mnemonic(optable, &test_op, "DCR");
        check(test_op.instr == LEX_DCR);
        check(strncmp(test_op.mnemonic, "DCR", 3) == 0);

        opcode_table_find_mnemonic(optable, &test_op, "INVALID");
        check(test_op.instr == LEX_INVALID);
        check(strncmp(test_op.mnemonic, "INVALID", 7) == 0);

        opcode_table_destroy(optable);
    }

}
