/*
 * TEST_LEXER
 * Unit tests for lexer object
 *
 * Stefan Wong 2020
 */


#include <stdio.h>
#include "assembler.h"
// NOTE: use check() for asserts

// testing framework
#include "bdd-for-c.h"


spec("Lexer")
{
    static const char test_filename[] = "asm/test_lexer.asm";

    it("Reads the asm file correctly")
    {
        Lexer* lexer = create_lexer();
        check(lexer != NULL);
        check(lexer->cur_pos == 0);
        check(lexer->cur_line == 1);
        check(lexer->cur_char == '\0');
        check(lexer->token_buf_ptr == 0);
        check(lexer->text_addr == 0);
        check(lexer->data_addr == 0);
        check(lexer->token_buf[0] == '\0');

        // Lexer object is fine, load an assembler file from disk
         
        // make a buffer where we can place the text from the source file
        char* src_buf;
        int src_file_size = asm_get_file_size(test_filename);

        fprintf(stdout, "[%s] file [%s] is %d bytes long\n", __func__, test_filename, src_file_size);

        src_buf = malloc(src_file_size * sizeof(char));
        check(src_buf != NULL);

        int status = asm_read_file(test_filename, src_buf, src_file_size); 
        check(status == 0); // make sure we read correctly

        fprintf(stdout, "[%s] source file [%s] contents:\n\n", __func__, test_filename);
        for(int c = 0; c < src_file_size; ++c)
        {
            fprintf(stdout, "%c", src_buf[c]);
        }
        fprintf(stdout, "\n\n");

        free(src_buf);
        destroy_lexer(lexer);
    }

    it("Scans tokens into the token buffer")
    {
        Lexer* lexer = create_lexer();

        // get the source text into a buffer
        // TODO: probably factor this out into a before_each or something
        char* src_buf;
        int src_file_size = asm_get_file_size(test_filename);

        fprintf(stdout, "[%s] file [%s] is %d bytes long\n", __func__, test_filename, src_file_size);

        src_buf = malloc(src_file_size * sizeof(char));
        check(src_buf != NULL);

        int status = asm_read_file(test_filename, src_buf, src_file_size); 
        check(status == 0); // make sure we read correctly

        // Scan a token and see what it is. We should be skipping over a lot of whitespace, so so the line number of first line should be 8

        free(src_buf);
        destroy_lexer(lexer);
    }

    it("Correctly skips comments")
    {
        Lexer* lexer = create_lexer();

        char* src_buf;
        int src_file_size = asm_get_file_size(test_filename);

        fprintf(stdout, "[%s] file [%s] is %d bytes long\n", __func__, test_filename, src_file_size);

        src_buf = malloc(src_file_size * sizeof(char));
        check(src_buf != NULL);

        int status = asm_read_file(test_filename, src_buf, src_file_size); 
        check(status == 0); // make sure we read correctly

        check(lexer->cur_line == 1);
        // Skip over the comments
        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        fprintf(stdout, "[%s] cur_line = %d\n", __func__, lexer->cur_line);
        check(lexer->cur_line == 2);

        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        fprintf(stdout, "[%s] cur_line = %d\n", __func__, lexer->cur_line);
        check(lexer->cur_line == 3);

        lex_skip_comment(lexer, src_buf, (size_t) src_file_size);
        fprintf(stdout, "[%s] cur_line = %d\n", __func__, lexer->cur_line);
        check(lexer->cur_line == 4);



        free(src_buf);
        destroy_lexer(lexer);
    }

    //it("Scans through the ROM file and collects each token")
    //{

    //}

}
