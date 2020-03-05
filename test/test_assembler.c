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

static const char mov_test_filename[] = "asm/test_mov.asm";
static const char arith_test_filename[] = "asm/test_arith.asm";
static const char byte_list_filename[]  = "asm/test_byte_list.asm";
static const char long_instr_filename[]  = "asm/test_long_instr.asm";

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
        // TODO: refactor how STAX, LDAX are assembled
        cur_instr = instr_vector_get(instr_vec, 15);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0010);
        //check(cur_instr->instr == 0x0A);

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

        // Now assemble
        status = assembler_assem(assembler);
        fprintf(stdout, "[%s] assembly status = %d\n", __func__, status);
        check(status == -1);        // last DB instruction is invalid

        InstrVector* instr_vec = assembler_get_instr_vector(assembler);
        check(instr_vec != NULL);

        fprintf(stdout, "[%s] there are %d instructions in buffer\n",
               __func__, instr_vector_size(instr_vec));
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            Instr* cur_instr = instr_vector_get(instr_vec, i);
            check(cur_instr != NULL);
            //fprintf(stdout, "Instr %02d : ", i+1);
            //instr_print(cur_instr);
            //fprintf(stdout, "\n");
        }

        // DB instruction arguments appear 'inline' in the output 
        // assembly.

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
}
