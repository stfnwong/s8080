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
    static const char test_filename[] = "asm/test_lexer.asm";

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
        for(int a = 0; a < 3; ++a)
            check(lexer->text_seg->reg[a] == '\0');

        // Check the opcode table
        check(lexer->op_table != NULL);

        // Check the SourceInfo object
        check(lexer->source_repr == NULL);

        // Lexer object is fine, load an assembler file from disk
        int status = lex_read_file(lexer, test_filename);
        check(status == 0);

        // We should also have a new source_repr object in the Lexer
        check(lexer->source_repr != NULL);
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
        check(lexer->text_seg->reg[2] == '\0');
        // The source is an immediate
        check(lexer->text_seg->has_immediate == 1);
        check(lexer->text_seg->immediate == 0x77);

        // Lex the next line (INR A)
        lex_line(lexer);
        line_info_print(lexer->text_seg);
        fprintf(stdout, "\n");
        check(lexer->text_seg->label_str == NULL);
        // followed by the instruction INR
        check(lexer->text_seg->opcode->instr == LEX_INR);
        check(strncmp(lexer->text_seg->opcode->mnemonic, "INR", 3) == 0);
        check(lexer->text_seg->reg[0] == 'A');
        check(lexer->text_seg->reg[1] == '\0');
        check(lexer->text_seg->reg[2] == '\0');

        check(lexer->text_seg->has_immediate == 0);
        check(lexer->text_seg->immediate == 0);


        //lex_line(lexer);
        //lex_line(lexer);
        //lex_line(lexer);

        lexer_destroy(lexer);
    } 
}
