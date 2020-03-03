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

    it("Should allocate instruction memory when a SourceInfo object is attached")
    {
        // What happens if I randomly make a lexer here?
        Lexer* lexer;
        lexer = lexer_create();
        check(lexer != NULL);
        check(lexer->text_seg != NULL);

        // Get an assembler object
        Assembler* assem;
        SourceInfo* test_repr;
        int num_lines = 32;
        int status;

        assem = assembler_create();
        check(assem != NULL);
        check(assem->instr_buf != NULL);
        check(assem->src_repr == NULL);
        check(assem->cur_line == 0);
        check(assembler_verbose(assem) == 0);

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

        assembler_destroy(assem);

        lexer_destroy(lexer);       // what is the problem here...?
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
        //check(lexer->sym_table->size == 0);
        //lex_set_verbose(lexer);

        // Lex the file, then take the src_repr and give it 
        // to the assembler
        lex_all(lexer);


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
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            Instr* cur_instr = instr_vector_get(instr_vec, i);
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // TODO : create reference assembly to test against
        //Instr* cur_instr; 

        // TODO : how big do we expect the buffer to be after assembly?
        fprintf(stdout, "[%s] destroying assembler...\n", __func__);
        assembler_destroy(assembler);
        fprintf(stdout, "[%s] destroying lexer...\n", __func__);
        lexer_destroy(lexer);       // what is the problem here...?
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

        // Lex the file, then take the src_repr and give it 
        // to the assembler
        lex_all(lexer);

        // TODO ; debug only, remove 
        for(int i = 0; i < lexer->source_repr->size; ++i)
        {
            LineInfo* l = source_info_get_idx(lexer->source_repr, i);
            check(l != NULL);
            line_info_print(l);
        }

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
        for(int i = 0; i < instr_vector_size(instr_vec); ++i)
        {
            Instr* cur_instr = instr_vector_get(instr_vec, i);
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        Instr* cur_instr;

        // ARITH_INSTR: ADD C
        cur_instr = instr_vector_get(instr_vec, 0);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0004);

        /// SUB A
        cur_instr = instr_vector_get(instr_vec, 1);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0008);

        /// ADI 7
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x000B);

        /// ORA B
        cur_instr = instr_vector_get(instr_vec, 2);
        check(cur_instr != NULL);
        check(cur_instr->size == 1);
        check(cur_instr->addr == 0x0010);

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

        // Lex the file, then take the src_repr and give it 
        // to the assembler
        lex_all(lexer);

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
            fprintf(stdout, "Instr %02d : ", i+1);
            instr_print(cur_instr);
            fprintf(stdout, "\n");
        }

        // DB instruction arguments appear 'inline' in the output 
        // assembly.


        assembler_destroy(assembler);
        lexer_destroy(lexer); 
    }
}
