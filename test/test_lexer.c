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
        for(int a = 0; a < LINE_INFO_NUM_REG; ++a)
            check(lexer->text_seg->reg[a] == '\0');

        // Check the opcode table
        check(lexer->op_table != NULL);
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
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == '\0');
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
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == '\0');

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
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == 'A');

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
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == '\0');

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
        check(lexer->text_seg->reg[0] == 'C');
        check(lexer->text_seg->reg[1] == 'B');

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
        check(lexer->text_seg->reg[0] == 'C');
        check(lexer->text_seg->reg[1] == '\0');

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

        // Skip the first few lines (which are comments and so on)
        while(lexer->cur_line < 5)
            lex_advance(lexer);

        lexer->verbose = 1;

        // MOVE_INSTR: MOV A, B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str != NULL);
        check(lexer->text_seg->label_str_len == 10);
        check(strncmp(lexer->text_seg->label_str, "MOVE_INSTR", 10) == 0);
        // MOV B, C
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MOV);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MOV", 3) == 0);
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == 'C');
        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);

        // MOV A, M
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MOV);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MOV", 3) == 0);
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == 'M');
        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);

        // PUSH D
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_PUSH);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "PUSH", 4) == 0);
        check(lexer->text_seg->reg[0] == 'D');
        check(lexer->text_seg->reg[1] == '\0');
        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);

        // MOV E, A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MOV);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MOV", 3) == 0);
        check(lexer->text_seg->reg[0] == 'E');
        check(lexer->text_seg->reg[1] == 'A');
        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);

        // MVI C, 2
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_MVI);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "MVI", 3) == 0);
        check(lexer->text_seg->reg[0] == 'C');
        check(lexer->text_seg->reg[1] == '\0');
        check(lexer->text_seg->has_immediate == 1);
        check(lexer->text_seg->immediate == 0x2);

        // POP D
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_POP);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "POP", 3) == 0);
        check(lexer->text_seg->reg[0] == 'D');
        check(lexer->text_seg->reg[1] == '\0');
        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);
        
        // clean up
        lexer_destroy(lexer);
    }

    it("Lexes the move file into a SourceInfo when lex_all() is called")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, mov_test_filename);
        check(status == 0);

        // Lex the file 
        lex_all(lexer);
        // Now check the internal SourceInfo
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                mov_test_filename, 
                lexer->source_repr->size
        );

        // Print each element
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            LineInfo* cur_line = source_info_get_idx(
                    lexer->source_repr,
                    l
            );
            check(cur_line != NULL);

            // Just for debugging
            line_info_print_instr(cur_line);
            fprintf(stdout, "\n");
        }

        // clean up
        lexer_destroy(lexer);
    }

    it("Should lex all the arithmetic instructions correctly")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, arith_test_filename);
        check(status == 0);
        lexer->verbose = 1;

        // Skip the first few lines (which are comments and so on)
        while(lexer->cur_line < 5)
            lex_advance(lexer);
        
        // ARITH_INSTR: ADD A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str != NULL);
        check(lexer->text_seg->label_str_len == 11);
        check(strncmp(lexer->text_seg->label_str, "ARITH_INSTR", 10) == 0);
        // ADD A section
        check(lexer->text_seg->opcode->instr == LEX_ADD);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "ADD", 3) == 0);
        check(lexer->text_seg->reg[0] == 'C');
        check(lexer->text_seg->reg[1] == '\0');

        // SUB B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_SUB);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "SUB", 3) == 0);
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == '\0');

        // ADI 7
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_ADI);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "ADI", 3) == 0);
        check(lexer->text_seg->has_immediate == 1);
        check(lexer->text_seg->immediate == 0x7);
        check(lexer->text_seg->reg[0] == '\0');
        check(lexer->text_seg->reg[1] == '\0');

        // ORA A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_ORA);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "ORA", 3) == 0);
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == '\0');

        // XRA D
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_XRA);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "XRA", 3) == 0);
        check(lexer->text_seg->reg[0] == 'D');
        check(lexer->text_seg->reg[1] == '\0');

        // ANA H
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_ANA);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "ANA", 3) == 0);
        check(lexer->text_seg->reg[0] == 'H');
        check(lexer->text_seg->reg[1] == '\0');

        // ADC E
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_ADC);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "ADC", 3) == 0);
        check(lexer->text_seg->reg[0] == 'E');
        check(lexer->text_seg->reg[1] == '\0');

        // CMP A
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_CMP);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "CMP", 3) == 0);
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == '\0');

        // SBB L
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_SBB);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "SBB", 3) == 0);
        check(lexer->text_seg->reg[0] == 'L');
        check(lexer->text_seg->reg[1] == '\0');

        // DAD H
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_DAD);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "DAD", 3) == 0);
        check(lexer->text_seg->reg[0] == 'H');
        check(lexer->text_seg->reg[1] == '\0');

        // INR D
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_INR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INR", 3) == 0);
        check(lexer->text_seg->reg[0] == 'D');
        check(lexer->text_seg->reg[1] == '\0');

        // INX D
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_INX);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INX", 3) == 0);
        check(lexer->text_seg->reg[0] == 'D');
        check(lexer->text_seg->reg[1] == '\0');

        // INR H
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_INR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INR", 3) == 0);
        check(lexer->text_seg->reg[0] == 'H');
        check(lexer->text_seg->reg[1] == '\0');

        // INX H
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_INX);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INX", 3) == 0);
        check(lexer->text_seg->reg[0] == 'H');
        check(lexer->text_seg->reg[1] == '\0');

        // DAD B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_DAD);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "DAD", 3) == 0);
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == '\0');

        // LDAX B
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");

        check(lexer->text_seg->label_str == NULL);
        check(lexer->text_seg->opcode->instr == LEX_LDAX);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "LDAX", 4) == 0);
        check(lexer->text_seg->reg[0] == 'B');
        check(lexer->text_seg->reg[1] == '\0');

        // clean up
        lexer_destroy(lexer);
    }

    it("Lexes the arithmetic file into a SourceInfo when lex_all() is called")
    {
        Lexer* lexer = lexer_create();

        int status = lex_read_file(lexer, arith_test_filename);
        check(status == 0);
        //lexer->verbose = 1;

        // Lex the file 
        lex_all(lexer);
        // Now check the internal SourceInfo
        fprintf(stdout, "[%s] source info for file [%s] contains %d lines\n", 
                __func__, 
                arith_test_filename, 
                lexer->source_repr->size
        );

        // Print each element
        for(int l = 0; l < lexer->source_repr->size; ++l)
        {
            LineInfo* cur_line = source_info_get_idx(
                    lexer->source_repr,
                    l
            );
            check(cur_line != NULL);

            // Just for debugging
            line_info_print_instr(cur_line);
            fprintf(stdout, "\n");
        }

        // clean up
        lexer_destroy(lexer);
    }

    it("Resolves symbols in move assembly source")
    {
        Lexer* lexer = lexer_create();
        Symbol* out_sym;

        int status = lex_read_file(lexer, mov_test_filename);
        check(status == 0);
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
            fprintf(stdout, "[%s] 0x%04X : %s\n", __func__, out_sym->addr, out_sym->sym);
        }

        check(lexer->sym_table->size == 1);

        // clean up
        lexer_destroy(lexer);
    }
}
