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

    // Read source text into a buffer
    before()
    {
        // make a buffer where we can place the text from the source file
        src_file_size = lex_get_file_size(test_filename);
        fprintf(stdout, "[%s] file [%s] is %d bytes long\n", __func__, test_filename, src_file_size);
        src_buf = malloc(src_file_size * sizeof(char));
        check(src_buf != NULL);
        int status = lex_read_file(test_filename, src_buf, src_file_size); 
        check(status == 0); // make sure we read correctly
    }

    // Clean up source text memory
    after()
    {
        free(src_buf);
    }

    it("Reads the asm file correctly")
    {
        Lexer* lexer = create_lexer();
        check(lexer != NULL);
        check(lexer->cur_pos == 0);
        check(lexer->cur_line == 1);
        check(lexer->cur_col == 1);
        check(lexer->cur_char == '\0');
        check(lexer->token_buf_ptr == 0);
        check(lexer->text_addr == 0);
        check(lexer->data_addr == 0);
        check(lexer->token_buf[0] == '\0');

        // Lexer object is fine, load an assembler file from disk
         
        fprintf(stdout, "[%s] source file [%s] contents:\n\n", __func__, test_filename);
        for(int c = 0; c < src_file_size; ++c)
        {
            fprintf(stdout, "%c", src_buf[c]);
        }
        fprintf(stdout, "\n\n");

        destroy_lexer(lexer);
    }

    // Check we can skip comments correctly
    it("Correctly skips comments")
    {
        Lexer* lexer = create_lexer();

        check(lexer->cur_line == 1);
        // Skip over the comments
        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        check(lexer->cur_line == 2);

        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        check(lexer->cur_line == 3);

        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        check(lexer->cur_line == 4);

        destroy_lexer(lexer);
    }

    // Check we can perform token scans correctly
    it("Scans tokens into the token buffer")
    {
        Lexer* lexer = create_lexer();
        Token* cur_token = create_token();
        // Scan a token and see what it is. We should be skipping over a lot of whitespace, so so the line number of first line should be 8

        while(lexer->cur_line < 7)
            lex_advance(lexer, src_buf, src_file_size);

        check(lexer->cur_line == 7);
        // Starting from line 7 we expect to see MOVI: MVI A 077H
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "MOVI", 4) == 0)

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "MVI", 3) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "077H", 4) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(lexer->cur_line == 8);
        check(strncmp(lexer->token_buf, "INR", 3) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        check(lexer->cur_line == 9);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "MOV", 3) == 0);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "B", 1) == 0);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "A", 1) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "INR", 3) == 0);
        check(lexer->cur_line == 10);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "B", 1) == 0);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "MOV", 3) == 0);
        
        check(lexer->cur_line == 11);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "C", 1) == 0);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "B", 1) == 0);

        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "DCR", 3) == 0);
        check(lexer->cur_line == 12);
        lex_next_token(lexer, cur_token, src_buf, (size_t) src_file_size); 
        check(strncmp(lexer->token_buf, "C", 1) == 0);

        destroy_token(cur_token);
        destroy_lexer(lexer);
    }
}
