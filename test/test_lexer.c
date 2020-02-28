/*
 * TEST_LEXER
 * Unit tests for lexer object
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

spec("Lexer")
{
    static const char test_filename[]       = "asm/test_lexer.asm";
    static const char mov_test_filename[]   = "asm/test_mov.asm";
    static const char arith_test_filename[] = "asm/test_arith.asm";
    static const char jmp_test_filename[]   = "asm/test_jmp.asm";
    static const char byte_list_filename[]  = "asm/test_byte_list.asm";

    it("Should initialize correctly")
    {
        Lexer* lexer = lexer_create();

        check(lexer != NULL);
        check(lexer->cur_pos == 0);
        check(lexer->cur_line == 1);
        check(lexer->cur_col == 1);
        check(lexer->cur_char == '\0');
        check(lexer->token_buf_ptr == 0);
        check(lexer->text_addr == 0);
        check(lexer->data_addr == 0);
        check(lexer->token_buf[0] == '\0');
        check(lexer->verbose == 0);
        check(lexer->text_seg != NULL);

        // Check the values in the text segment structure
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->label_str_len == 0);
        check(lexer->text_seg->error == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->text_seg->has_immediate == 0);
        for(int a = 0; a < LINE_INFO_NUM_REG; ++a)
            check(lexer->text_seg->reg[a] == REG_NONE);

        // Check the opcode table
        check(lexer->op_table != NULL);
        // Check the directive table 
        check(lexer->dir_table != NULL);
        // Check the SourceInfo object
        check(lexer->source_repr == NULL);
        // Check the SymbolTable
        check(lexer->sym_table == NULL);

        // Lexer object is fine, load an assembler file from disk
        int status = lex_read_file(lexer, test_filename);
        check(status == 0);

        // We should also have a new source_repr object in the Lexer
        check(lexer->source_repr != NULL);
        check(lexer->sym_table != NULL);
        check(lexer->source_repr->size == 0);
        check(lexer->source_repr->cur_line == 0);
        check(lexer->source_repr->max_size == 12);      // since there are 12 lines in the source file
         
        fprintf(stdout, "[%s] source file [%s] contents:\n\n", __func__, test_filename);
        for(int c = 0; c < lexer->src_len; ++c)
        {
            fprintf(stdout, "%c", lexer->src[c]);
        }
        fprintf(stdout, "\n\n");

        lexer_destroy(lexer);
    }

    // Check we can skip comments correctly
    it("Should correctly skips comments")
    {
        int status;
        Lexer* lexer = lexer_create();

        status = lex_read_file(lexer, test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);

        check(lexer->cur_line == 1);
        // Skip over the comments
        lex_skip_comment(lexer);
        check(lexer->cur_line == 2);

        lex_skip_comment(lexer);
        check(lexer->cur_line == 3);

        lex_skip_comment(lexer);
        check(lexer->cur_line == 4);

        lexer_destroy(lexer);
    }

    // Check we can perform token scans correctly
    it("Scans tokens into the token buffer")
    {
        Lexer* lexer = lexer_create();
        Token* cur_token = create_token();

        int status = lex_read_file(lexer, test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);

        // Skip the first few lines (which are comments and so on)
        while(lexer->cur_line < 7)
            lex_advance(lexer);

        check(lexer->cur_line == 7);
        // Starting from line 7 we expect to see MOVI: MVI A 077H
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "MOVI", 4) == 0)

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "MVI", 3) == 0);

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "077H", 4) == 0);

        lex_next_token(lexer, cur_token);
        check(lexer->cur_line == 8);
        check(strncmp(lexer->token_buf, "INR", 3) == 0);

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        check(lexer->cur_line == 9);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "MOV", 3) == 0);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "B", 1) == 0);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "INR", 3) == 0);
        check(lexer->cur_line == 10);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "B", 1) == 0);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "MOV", 3) == 0);
        
        check(lexer->cur_line == 11);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "C", 1) == 0);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "B", 1) == 0);

        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "DCR", 3) == 0);
        check(lexer->cur_line == 12);
        lex_next_token(lexer, cur_token);
        check(strncmp(lexer->token_buf, "C", 1) == 0);

        destroy_token(cur_token);
        lexer_destroy(lexer);
    }

    it("Should convert a line of assembly source into a LineInfo structure")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);

        // Skip the first few lines (which are comments and so on)
        while(lexer->cur_line < 7)
            lex_advance(lexer);

        lexer->verbose = 1;
        
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");
        // Lexing the first line (MOVI: MVI, A, 077H
        // We expect to have the label MOVI
        check(lexer->text_seg->label_str != NULL);
        check(lexer->text_seg->label_str_len == 4);
        check(strncmp(lexer->text_seg->label_str, "MOVI", 4) == 0);
        // followed by the instruction MVI
        check(lexer->text_seg->opcode->instr == LEX_MVI);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MVI", 3) == 0);
        // the dest register is A
        check(lexer->text_seg->reg[0] == REG_A);
        check(lexer->text_seg->reg[1] == REG_NONE);
        // The source is an immediate
        check(lexer->text_seg->has_immediate == 1);
        check(lexer->text_seg->immediate == 0x77);
        check(lexer->source_repr->size == 1);

        // INR A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");
        check(lexer->text_seg->label_str == NULL);
        // followed by the instruction INR
        check(lexer->text_seg->opcode->instr == LEX_INR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INR", 3) == 0);
        check(lexer->text_seg->reg[0] == REG_A);
        check(lexer->text_seg->reg[1] == REG_NONE);

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->source_repr->size == 2);

        // MOV B,A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");
        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MOV);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MOV", 3) == 0);
        check(lexer->text_seg->reg[0] == REG_B);
        check(lexer->text_seg->reg[1] == REG_A);

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->source_repr->size == 3);

        // INR B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_INR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INR", 3) == 0);
        check(lexer->text_seg->reg[0] == REG_B);
        check(lexer->text_seg->reg[1] == REG_NONE);

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->source_repr->size == 4);

        // MOV C,B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MOV);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MOV", 3) == 0);
        check(lexer->text_seg->reg[0] == REG_C);
        check(lexer->text_seg->reg[1] == REG_B);

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->source_repr->size == 5);

        // DCR C
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_DCR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "DCR", 3) == 0);
        check(lexer->text_seg->reg[0] == REG_C);
        check(lexer->text_seg->reg[1] == REG_NONE);

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        check(lexer->source_repr->size == 6);

        // clean up
        lexer_destroy(lexer);
    } 

    it("Should lex all the move instructions correctly")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, mov_test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);

        // Lex the file 
        lex_all(lexer);
        // Now check the internal SourceInfo
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                mov_test_filename, 
                lexer->source_repr->size
        );

        // Print each element
        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // MOVE_INSTR: MOV A, B
        cur_line = source_info_get_idx(lexer->source_repr, 0);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 10);
        check(strncmp(cur_line->label_str, "MOVE_INSTR", 10) == 0);
        // MOV B, C
        cur_line = source_info_get_idx(lexer->source_repr, 1);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_MOV);
        check(strncmp(cur_line->opcode->mnemonic, "MOV", 3) == 0);
        check(cur_line->reg[0] == REG_B);
        check(cur_line->reg[1] == REG_C);
        check(cur_line->has_immediate == 0);
        check(cur_line->immediate == 0);

        // MOV A, M
        cur_line = source_info_get_idx(lexer->source_repr, 2);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_MOV);
        check(strncmp(cur_line->opcode->mnemonic, "MOV", 3) == 0);
        check(cur_line->reg[0] == REG_A);
        check(cur_line->reg[1] == REG_M);
        check(cur_line->has_immediate == 0);
        check(cur_line->immediate == 0);

        // PUSH D
        cur_line = source_info_get_idx(lexer->source_repr, 3);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_PUSH);
        check(strncmp(cur_line->opcode->mnemonic, "PUSH", 4) == 0);
        check(cur_line->reg[0] == REG_D);
        check(cur_line->reg[1] == REG_NONE);
        check(cur_line->has_immediate == 0);
        check(cur_line->immediate == 0);

        // MOV E, A
        cur_line = source_info_get_idx(lexer->source_repr, 4);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_MOV);
        check(strncmp(cur_line->opcode->mnemonic, "MOV", 3) == 0);
        check(cur_line->reg[0] == REG_E);
        check(cur_line->reg[1] == REG_A);
        check(cur_line->has_immediate == 0);
        check(cur_line->immediate == 0);

        // MVI C, 2
        cur_line = source_info_get_idx(lexer->source_repr, 5);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_MVI);
        check(strncmp(cur_line->opcode->mnemonic, "MVI", 3) == 0);
        check(cur_line->reg[0] == REG_C);
        check(cur_line->reg[1] == REG_NONE);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0x2);

        // POP D
        cur_line = source_info_get_idx(lexer->source_repr, 6);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_POP);
        check(strncmp(cur_line->opcode->mnemonic, "POP", 3) == 0);
        check(cur_line->reg[0] == REG_D);
        check(cur_line->reg[1] == REG_NONE);
        check(cur_line->has_immediate == 0);
        check(cur_line->immediate == 0);
        
        // clean up
        lexer_destroy(lexer);
    }

    it("Should lex all the arithmetic instructions correctly")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, arith_test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);

        // lex the file
        lex_all(lexer);

        LineInfo* cur_line;
        // Check that there aren't null lines in the source repr
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // get the first line
        cur_line = source_info_get_idx(lexer->source_repr, 0);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 11);
        check(strncmp(cur_line->label_str, "ARITH_INSTR", 11) == 0);
        // ADD A section
        check(cur_line->opcode->instr == LEX_ADD);
        check(strncmp(cur_line->opcode->mnemonic, "ADD", 3) == 0);
        check(cur_line->reg[0] == REG_C);
        check(cur_line->reg[1] == REG_NONE);

        // SUB B
        cur_line = source_info_get_idx(lexer->source_repr, 1);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_SUB);
        check(strncmp(cur_line->opcode->mnemonic, "SUB", 3) == 0);
        check(cur_line->reg[0] == REG_A);
        check(cur_line->reg[1] == REG_NONE);

        // ADI 7
        cur_line = source_info_get_idx(lexer->source_repr, 2);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_ADI);
        check(strncmp(cur_line->opcode->mnemonic, "ADI", 3) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0x7);
        check(cur_line->reg[0] == REG_NONE);
        check(cur_line->reg[1] == REG_NONE);

        // ORA A
        cur_line = source_info_get_idx(lexer->source_repr, 3);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_ORA);
        check(strncmp(cur_line->opcode->mnemonic, "ORA", 3) == 0);
        check(cur_line->reg[0] == REG_B);
        check(cur_line->reg[1] == REG_NONE);

        // XRA D
        cur_line = source_info_get_idx(lexer->source_repr, 4);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_XRA);
        check(strncmp(cur_line->opcode->mnemonic, "XRA", 3) == 0);
        check(cur_line->reg[0] == REG_D);
        check(cur_line->reg[1] == REG_NONE);

        // ANA H
        cur_line = source_info_get_idx(lexer->source_repr, 5);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_ANA);
        check(strncmp(cur_line->opcode->mnemonic, "ANA", 3) == 0);
        check(cur_line->reg[0] == REG_H);
        check(cur_line->reg[1] == REG_NONE);

        // ADC E
        cur_line = source_info_get_idx(lexer->source_repr, 6);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_ADC);
        check(strncmp(cur_line->opcode->mnemonic, "ADC", 3) == 0);
        check(cur_line->reg[0] == REG_E);
        check(cur_line->reg[1] == REG_NONE);

        // CMP A
        cur_line = source_info_get_idx(lexer->source_repr, 7);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_CMP);
        check(strncmp(cur_line->opcode->mnemonic, "CMP", 3) == 0);
        check(cur_line->reg[0] == REG_A);
        check(cur_line->reg[1] == REG_NONE);

        // SBB L
        cur_line = source_info_get_idx(lexer->source_repr, 8);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_SBB);
        check(strncmp(cur_line->opcode->mnemonic, "SBB", 3) == 0);
        check(cur_line->reg[0] == REG_L);
        check(cur_line->reg[1] == REG_NONE);

        // DAD H
        cur_line = source_info_get_idx(lexer->source_repr, 9);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_DAD);
        check(strncmp(cur_line->opcode->mnemonic, "DAD", 3) == 0);
        check(cur_line->reg[0] == REG_H);
        check(cur_line->reg[1] == REG_NONE);

        // INR D
        cur_line = source_info_get_idx(lexer->source_repr, 10);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_INR);
        check(strncmp(cur_line->opcode->mnemonic, "INR", 3) == 0);
        check(cur_line->reg[0] == REG_D);
        check(cur_line->reg[1] == REG_NONE);

        // INX D
        cur_line = source_info_get_idx(lexer->source_repr, 11);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_INX);
        check(strncmp(cur_line->opcode->mnemonic, "INX", 3) == 0);
        check(cur_line->reg[0] == REG_D);
        check(cur_line->reg[1] == REG_NONE);

        // INR H
        cur_line = source_info_get_idx(lexer->source_repr, 12);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_INR);
        check(strncmp(cur_line->opcode->mnemonic, "INR", 3) == 0);
        check(cur_line->reg[0] == REG_H);
        check(cur_line->reg[1] == REG_NONE);

        // INX H
        cur_line = source_info_get_idx(lexer->source_repr, 13);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_INX);
        check(strncmp(cur_line->opcode->mnemonic, "INX", 3) == 0);
        check(cur_line->reg[0] == REG_H);
        check(cur_line->reg[1] == REG_NONE);

        // DAD B
        cur_line = source_info_get_idx(lexer->source_repr, 14);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_DAD);
        check(strncmp(cur_line->opcode->mnemonic, "DAD", 3) == 0);
        check(cur_line->reg[0] == REG_B);
        check(cur_line->reg[1] == REG_NONE);

        // LDAX B
        cur_line = source_info_get_idx(lexer->source_repr, 15);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_LDAX);
        check(strncmp(cur_line->opcode->mnemonic, "LDAX", 4) == 0);
        check(cur_line->reg[0] == REG_B);
        check(cur_line->reg[1] == REG_NONE);

        // clean up
        lexer_destroy(lexer);
    }

    it("Should lex all the jump instructions correctly")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, jmp_test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        // Set the start address to something memorable 
        lex_set_text_start_addr(lexer, 0xBEEF);

        lex_all(lexer);
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                jmp_test_filename, 
                lexer->source_repr->size
        );

        // Ensure there are no NULL elements in source repr
        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // SOME_LABEL: ANI 0
        cur_line = source_info_get_idx(lexer->source_repr, 0);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 10);   
        check(strncmp(cur_line->label_str, "SOME_LABEL", 10) == 0);
        // ADD A section
        check(cur_line->opcode->instr == LEX_ANI);
        check(strncmp(cur_line->opcode->mnemonic, "ANI", 3) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0);

        // OTHER_LABEL: ADI 7
        cur_line = source_info_get_idx(lexer->source_repr, 1);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
        
        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 11);   
        check(strncmp(cur_line->label_str, "OTHER_LABEL", cur_line->label_str_len) == 0);
        check(cur_line->opcode->instr == LEX_ADI);
        check(strncmp(cur_line->opcode->mnemonic, "ADI", 3) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 7);
        
        // JMP_INSTR: JMP SOME_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 2);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 9);   
        check(strncmp(cur_line->label_str, "JMP_INSTR", 9) == 0);
        check(cur_line->opcode->instr == LEX_JMP);
        check(strncmp(cur_line->opcode->mnemonic, "JMP", 3) == 0);

        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 10);
        check(strncmp(cur_line->symbol_str, "SOME_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF);

        // JMP OTHER_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 3);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_JMP);
        check(strncmp(cur_line->opcode->mnemonic, "JMP", 3) == 0);
        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 11);
        check(strncmp(cur_line->symbol_str, "OTHER_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF + 8);

        // JZ SOME_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 4);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_JZ);
        check(strncmp(cur_line->opcode->mnemonic, "JZ", 3) == 0);
        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 10);
        check(strncmp(cur_line->symbol_str, "SOME_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF);

        // JNC OTHER_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 5);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_JNC);
        check(strncmp(cur_line->opcode->mnemonic, "JNC", 3) == 0);
        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 11);
        check(strncmp(cur_line->symbol_str, "OTHER_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF + 8);

        // JP SOME_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 6);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_JP);
        check(strncmp(cur_line->opcode->mnemonic, "JP", 3) == 0);
        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 10);
        check(strncmp(cur_line->symbol_str, "SOME_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF);

        // JC OTHER_LABEL
        cur_line = source_info_get_idx(lexer->source_repr, 7);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");

        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_JC);
        check(strncmp(cur_line->opcode->mnemonic, "JC", 3) == 0);
        check(cur_line->symbol_str != NULL);
        check(cur_line->symbol_str_len == 11);
        check(strncmp(cur_line->symbol_str, "OTHER_LABEL", cur_line->symbol_str_len) == 0);
        check(cur_line->has_immediate == 1);
        check(cur_line->immediate == 0xBEEF + 8);

        // clean up
        lexer_destroy(lexer);
    }

    it("Resolves symbols in move assembly source")
    {
        Lexer* lexer = lexer_create();
        Symbol* out_sym;

        int status = lex_read_file(lexer, mov_test_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->sym_table->size == 0);
        lexer->verbose = 1;
        // Lex the file 
        lex_all(lexer);
        // Now check the internal SourceInfo
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                mov_test_filename, 
                lexer->source_repr->size
        );

        // There should be a single symbol in the symbol table
        fprintf(stdout, "[%s] number of symbols in table : %d\n",
                __func__, lexer->sym_table->size
        );
        for(int s = 0; s < lexer->sym_table->size; ++s)
        {
            out_sym = symbol_table_get_idx(
                    lexer->sym_table,
                    s
            );
            check(out_sym != NULL);
            fprintf(stdout, "[%s] 0x%04X : %s (len = %ld)\n", __func__, out_sym->addr, out_sym->sym, strlen(out_sym->sym));
        }
        check(lexer->sym_table->size == 1);
        check(strncmp(out_sym->sym, "MOVE_INSTR", 11) == 0);
        check(out_sym->addr == 0x0);

        // clean up
        lexer_destroy(lexer);
    }

    it("Should lex DB instruction correctly")
    {
        Lexer* lexer = lexer_create();
        Symbol* out_sym;

        int status = lex_read_file(lexer, byte_list_filename);
        check(status == 0);
        check(lexer->text_seg->line_num == 0);
        check(lexer->text_seg->addr == 0);
        check(lexer->text_seg->label_str == NULL);
        check(lexer->sym_table->size == 0);
        lexer->verbose = 1;
        // Lex the file 
        status = lex_all(lexer);
        //check(status == -1);    // should be -1 here since final DB will fail
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                byte_list_filename, 
                lexer->source_repr->size
        );

        // Ensure there are no NULL elements in source repr
        LineInfo* cur_line;
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            cur_line = source_info_get_idx(lexer->source_repr, l);
            check(cur_line != NULL);
        }

        // TEST_ARGS : DB "SOME CHARACTER STRING", 0dh, 0ah, 03h
        cur_line = source_info_get_idx(lexer->source_repr, 0);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
        check(cur_line->label_str != NULL);
        check(cur_line->label_str_len == 9);   // +1 for null character
        check(strncmp(cur_line->label_str, "TEST_ARGS", 8) == 0);
        check(cur_line->symbol_str == NULL);

        check(cur_line->opcode->instr == LEX_DB);
        check(strncmp(cur_line->opcode->mnemonic, "DB", 2) == 0);

        fprintf(stdout, "[%s] %d args in byte list (%d bytes total)\n", 
                __func__, 
                byte_list_len(cur_line->byte_list),
                byte_list_total_bytes(cur_line->byte_list)
        );
        check(byte_list_len(cur_line->byte_list) == 4);

        // check the argument values 
        ByteNode* cur_node;
        // 1st arg
        cur_node = byte_list_get(cur_line->byte_list, 0);
        check(cur_node != NULL);
        check(cur_node->len == 21);
        uint8_t expected_str_data[] = {0x53, 0x4F, 0x4D, 0x45, 0x43, 0x48, 0x41, 
                                      0x52, 0x41, 0x43, 0x54, 0x45, 0x52, 0x20,
                                      0x53, 0x54, 0x52, 0x49, 0x4E, 0x47, 0x00};
        check(memcmp(cur_node->data, expected_str_data, 21) == 0);

        // 2nd arg
        cur_node = byte_list_get(cur_line->byte_list, 1);
        check(cur_node != NULL);
        check(cur_node->len == 1);
        check(cur_node->data[0] == 0xD);
        // 3rd arg
        cur_node = byte_list_get(cur_line->byte_list, 2);
        check(cur_node != NULL);
        check(cur_node->len == 1);
        check(cur_node->data[0] == 0xA);
        // 4th arg
        cur_node = byte_list_get(cur_line->byte_list, 3);
        check(cur_node != NULL);
        check(cur_node->len == 1);
        check(cur_node->data[0] == 0x3);
        // Any other arg is is invalid and will return NULL
        cur_node = byte_list_get(cur_line->byte_list, 4);
        check(cur_node == NULL);

        // DB 65h
        cur_line = source_info_get_idx(lexer->source_repr, 1);
        line_info_print_instr(cur_line);
        fprintf(stdout, "\n");
        check(cur_line->label_str == NULL);
        check(cur_line->opcode->instr == LEX_DB);
        check(strncmp(cur_line->opcode->mnemonic, "DB", 2) == 0);
        check(cur_line->symbol_str == NULL);
        check(byte_list_len(cur_line->byte_list) == 1);
        // Check the arg c
        cur_node = byte_list_get(cur_line->byte_list, 0);
        check(cur_node != NULL);
        check(cur_node->len == 1);
        check(cur_node->data[0] == 0x65);

        // Final invalid line will not lex and there will be no opcode
        cur_line = source_info_get_idx(lexer->source_repr, 2);
        line_info_print_instr(cur_line);
        check(cur_line->error == 1);

        // clean up
        lexer_destroy(lexer);
    }
}
