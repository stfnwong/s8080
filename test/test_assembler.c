/*
 * TEST_ASSEMBLER
 * Unit tests for Assembler object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "assembler.h"
#include "lexer.h"
#include "source.h"
// testing framework
#include "bdd-for-c.h"

static const char mov_test_filename[]      = "asm/test_mov.asm";
static const char arith_test_filename[]    = "asm/test_arith.asm";
static const char jump_test_filename[]     = "asm/test_jmp.asm";
static const char call_ret_test_filename[] = "asm/test_call_ret.asm";
static const char byte_list_filename[]     = "asm/test_byte_list.asm";
static const char long_instr_filename[]    = "asm/test_long_instr.asm";

spec("Assembler")
{
    it("Initializes correctly")
    {
        Assembler* assem;

        assem = assembler_create();
        check(assem != NULL);
        check(assem->instr_buf != NULL);
        check(assem->src_repr == NULL);
        check(assem->cur_line == 0);
        check(assembler_verbose(assem) == 0);
        // Note that the src_repr and instr_buf won't be initailized 
        // until a file is loaded
        assembler_destroy(assem);
    }

    it("Should assemble the mov instruction test file")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, mov_test_filename);
        check(status == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);

        // Lex source 
        lex_all(lexer);
        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);

        // Now assemble
        status = assembler_assem(assembler);
        check(status == 0);

        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);
        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));

        Instr* cur_instr;
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // MOVE_INSTR: MOV A, B
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x00);
        check(cur_instr->instr == 0x78);

        // MOV B, C
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x01);
        check(cur_instr->instr == 0x41);

        // MOV A, M
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x02);
        check(cur_instr->instr == 0x7E);

        // PUSH D 
        cur_instr = instr_vector_get(instr_vec, 3);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x03);
        check(cur_instr->instr == 0xD5);

        // MOV E, A
        cur_instr = instr_vector_get(instr_vec, 4);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x04);
        check(cur_instr->instr == 0x5F);

        // MVI C, 2
        cur_instr = instr_vector_get(instr_vec, 5);
        check(cur_instr != NULL);
        check(cur_instr->size == 2);
        check(cur_instr->addr == 0x05);
        check(cur_instr->instr == 0x0E02);

        // POP D
        cur_instr = instr_vector_get(instr_vec, 6);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x07);
        check(cur_instr->instr == 0xD1);

        // clean up
        assembler_destroy(assembler);
        lexer_destroy(lexer);       
    }

    it("Should assemble the arithmetic instruction test file")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);

        // Load the file 
        status = lex_read_file(lexer, arith_test_filename);
        check(status == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);
        //check(lexer->sym_table->size == 0);
        //lex_set_verbose(lexer);

        // Lex and assemble
        lex_all(lexer);
        fprintf(stdout, "[%s] %d lines in lexer source repr\n", __func__, lexer->source_repr->size);

        for(int i = 0; i < lexer->source_repr->size; ++i)
        {
            LineInfo* cur_line = source_info_get_idx(lexer->source_repr, i);
            check(cur_line != NULL);
            line_info_print_instr(cur_line);
            fprintf(stdout, "\n");
        }

        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);
        status = assembler_assem(assembler);
        check(status == 0);

        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);
        
        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));

        Instr* cur_instr;
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // ARITH_INSTR: ADD C
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0000);
        check(cur_instr->instr == 0x81);

        /// SUB A
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0001);
        check(cur_instr->instr == 0x97);

        /// ADI 7
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 2);
        check(cur_instr->addr == 0x0002);
        check(cur_instr->instr == 0xC607);

        /// ORA B
        cur_instr = instr_vector_get(instr_vec, 3);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0004);
        check(cur_instr->instr == 0xB0);

        // XRA D
        cur_instr = instr_vector_get(instr_vec, 4);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0005);
        check(cur_instr->instr == 0xAA);

        // ANA H 
        cur_instr = instr_vector_get(instr_vec, 5);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0006);
        check(cur_instr->instr == 0xA4);

        // ADC E 
        cur_instr = instr_vector_get(instr_vec, 6);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0007);
        check(cur_instr->instr == 0x8B);

        // CMP A 
        cur_instr = instr_vector_get(instr_vec, 7);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0008);
        check(cur_instr->instr == 0xBF);

        // SBB L 
        cur_instr = instr_vector_get(instr_vec, 8);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0009);
        check(cur_instr->instr == 0x9D);

        // DAD H 
        cur_instr = instr_vector_get(instr_vec, 9);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000A);
        //check(cur_instr->instr == 0x19);

        // INR D 
        cur_instr = instr_vector_get(instr_vec, 10);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        //check(cur_instr->addr == 0x000B);     // addr == 0 here?
        check(cur_instr->instr == 0x14);

        // INX D 
        cur_instr = instr_vector_get(instr_vec, 11);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000C);
        check(cur_instr->instr == 0x13);

        // INR H 
        cur_instr = instr_vector_get(instr_vec, 12);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000D);
        check(cur_instr->instr == 0x24);

        // INX H 
        cur_instr = instr_vector_get(instr_vec, 13);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000E);
        check(cur_instr->instr == 0x23);

        // DAD B 
        cur_instr = instr_vector_get(instr_vec, 14);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000F);
        check(cur_instr->instr == 0x09);

        // LDAX B 
        cur_instr = instr_vector_get(instr_vec, 15);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0010);
        check(cur_instr->instr == 0x08);

        // STAX D 
        cur_instr = instr_vector_get(instr_vec, 16);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0011);
        check(cur_instr->instr == 0x12);

        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }

    it("Should assemble the jump instruction test file")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, jump_test_filename);
        check(status == 0);

        // Lex source
        lex_all(lexer);

        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);

        // Now assemble
        status = assembler_assem(assembler);
        fprintf(stdout, "[%s] assembly status = %d\n", __func__, status);
        check(status == 0);

        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);

        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));

        Instr* cur_instr;
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // SOME_LABEL: ANI 0
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr != NULL);
        check(cur_instr->size == 2);
        check(cur_instr->addr == 0);
        check(cur_instr->instr == 0xE600);

        // OTHER_LABEL: ADI 7
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr != NULL);
        check(cur_instr->size == 2);
        check(cur_instr->addr == 0x2);
        check(cur_instr->instr == 0xC607);

        // JMP_INSTR: JMP SOME_LABEL (0x00)
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x4);
        check(cur_instr->instr == 0xC30000);

        // JMP OTHER_LABEL (0x2)
        cur_instr = instr_vector_get(instr_vec, 3);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x7);
        check(cur_instr->instr == 0xC30200);

        // JZ SOME_LABEL (0x0)
        cur_instr = instr_vector_get(instr_vec, 4);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0xA);
        check(cur_instr->instr == 0xCA0000);

        // JNC OTHER_LABEL (0x2)
        cur_instr = instr_vector_get(instr_vec, 5);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0xD);
        check(cur_instr->instr == 0xD20200);

        // JP SOME_LABEL (0x0)
        cur_instr = instr_vector_get(instr_vec, 6);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x10);
        check(cur_instr->instr == 0xF20000);

        // JC OTHER_LABEL (0x2)
        cur_instr = instr_vector_get(instr_vec, 7);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x13);
        check(cur_instr->instr == 0xDA0200);

        // JM SOME_LABEL (0x0)
        cur_instr = instr_vector_get(instr_vec, 8);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x16);
        check(cur_instr->instr == 0xFA0000);

        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }

    it("Should assemble the call/return instructions correctly")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, call_ret_test_filename);
        check(status == 0);

        // Lex source
        lex_all(lexer);
        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);
        status = assembler_assem(assembler);
        fprintf(stdout, "[%s] assembly status = %d\n", __func__, status);
        check(status == 0);

        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // Get instruction vector
        Instr* cur_instr;
        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);

        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));

        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // CNZ BEEFh
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x00);
        check(cur_instr->instr == 0xC4EFBE);

        // CZ BEEFh
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x03);
        check(cur_instr->instr == 0xCCEFBE);

        // CALL BEEFh
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x06);
        check(cur_instr->instr == 0xCDEFBE);

        // CC BEEFh
        cur_instr = instr_vector_get(instr_vec, 3);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x09);
        check(cur_instr->instr == 0xDCEFBE);

        // CPO BEEFh
        cur_instr = instr_vector_get(instr_vec, 4);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x0C);
        check(cur_instr->instr == 0xE4EFBE);

        // CPE BEEFh
        cur_instr = instr_vector_get(instr_vec, 5);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x0F);
        check(cur_instr->instr == 0xECEFBE);

        // CP BEEFh
        cur_instr = instr_vector_get(instr_vec, 6);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x12);
        check(cur_instr->instr == 0xF4EFBE);

        // CM BEEFh
        cur_instr = instr_vector_get(instr_vec, 7);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x15);
        check(cur_instr->instr == 0xFCEFBE);

        // RNZ 
        cur_instr = instr_vector_get(instr_vec, 8);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x18);
        check(cur_instr->instr == 0xC0);

        // RZ 
        cur_instr = instr_vector_get(instr_vec, 9);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x19);
        check(cur_instr->instr == 0xC8);

        // RET
        cur_instr = instr_vector_get(instr_vec, 10);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1A);
        check(cur_instr->instr == 0xC9);

        // RNC
        cur_instr = instr_vector_get(instr_vec, 11);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1B);
        check(cur_instr->instr == 0xD0);

        // RPO
        cur_instr = instr_vector_get(instr_vec, 12);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1C);
        check(cur_instr->instr == 0xE0);

        // RPE
        cur_instr = instr_vector_get(instr_vec, 13);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1D);
        check(cur_instr->instr == 0xE8);

        // RP
        cur_instr = instr_vector_get(instr_vec, 14);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1E);
        check(cur_instr->instr == 0xF0);

        // RM
        cur_instr = instr_vector_get(instr_vec, 15);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x1F);
        check(cur_instr->instr == 0xF8);
        

        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }

    it("Should assemble the DB instruction correctly")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, byte_list_filename);
        check(status == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);
        //check(lexer->sym_table->size == 0);
        //lex_set_verbose(lexer);

        // Lex source
        lex_all(lexer);
        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);

        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // Now assemble
        status = assembler_assem(assembler);
        fprintf(stdout, "[%s] assembly status = %d\n", __func__, status);
        check(status == 0);

        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);

        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));
        check(instr_vector_size(instr_vec) == 26);

        Instr* cur_instr;
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
        }

        // DB instruction arguments appear 'inline' in the output 
        // assembly. Note that there is no DB 'instruction', DB is 
        // more like an assembler directive.
        
        // TEST_ARGS : DB "SOME_CHARACTER_STRING", 0dh, 0ah, 03h
        const char expected_string[] = "SOME CHARACTER STRING\0";
        for(int i = 0; i < 22; ++i)  
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            check(cur_instr->size == 1);
            check(cur_instr->instr == (uint8_t) expected_string[i]);
            check(cur_instr->addr == i);
            fprintf(stdout, "%c", cur_instr->instr);
        }
        fprintf(stdout, "\n");

        // 0dh
        cur_instr = instr_vector_get(instr_vec, 22);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->instr == 0xD);
        check(cur_instr->addr == 22);

        // 0ah
        cur_instr = instr_vector_get(instr_vec, 23);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->instr == 0xA);
        check(cur_instr->addr == 23);

        // 03h
        cur_instr = instr_vector_get(instr_vec, 24);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->instr == 0x3);
        check(cur_instr->addr == 24);

        // DB 65h
        cur_instr = instr_vector_get(instr_vec, 25);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->instr == 0x65);
        check(cur_instr->addr == 25);

        // DB           (incomplete line)

        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }

    it("Assembles 3 byte instructions correctly")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, long_instr_filename);
        check(status == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);
        //check(lexer->sym_table->size == 0);
        //lex_set_verbose(lexer);

        // Lex source
        lex_all(lexer);
        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);

        // Now assemble
        status = assembler_assem(assembler);
        check(status == 0);     

        // Instruction check
        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);
        
        Instr* cur_instr;
        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // TEST_START: ADI 0
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr != NULL);
        check(cur_instr->size == 2);
        check(cur_instr->addr == 0x0000);
        check(cur_instr->instr == 0xC600);
        
        // LXI B 250h
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x0002);
        check(cur_instr->instr == 0x015002);    // note that byte order is reversed

        // LXI D 100h
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x0005);
        check(cur_instr->instr == 0x110001);

        // LXI H 5
        cur_instr = instr_vector_get(instr_vec, 3);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x0008);
        check(cur_instr->instr == 0x210500);

        // LXI SP 22h
        cur_instr = instr_vector_get(instr_vec, 4);
        check(cur_instr != NULL);
        check(cur_instr->size == 3);
        check(cur_instr->addr == 0x000B);
        check(cur_instr->instr == 0x312200);

        // cleanup
        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }

    it("Should write valid binaries to disk")
    {
        Lexer* lexer;
        Assembler* assembler;
        int status;

        //// Get an assembler object
        assembler = assembler_create();
        check(assembler != NULL);

        // Get a Lexer object
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Load the file 
        status = lex_read_file(lexer, byte_list_filenam);  // NOTE: I guess its not super important which file this is for now...
        check(status == 0);

        // Lex source
        lex_all(lexer);
        // Assemble source
        assembler_set_repr(assembler, lexer->source_repr);
        check(assembler->instr_buf != NULL);
        assembler_set_verbose(assembler);

        // Now assemble
        status = assembler_assem(assembler);
        check(status == 0);     

        status = assembler_write(assembler, "test_asm_out.asm"); 
        check(status != -1);

        // cleanup
        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }
}
