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
            check(lexer->text_seg->args[a] == '\0');

        // Lexer object is fine, load an assembler file from disk
        int status = lex_read_file(lexer, test_filename);
        check(status == 0);
         
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
        // Scan a token and see what it is. We should be skipping over a lot of whitespace, so so the line number of first line should be 8

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
}
