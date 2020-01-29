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

    it("Reads the ROM file correctly")
    {
        Lexer* lexer = create_lexer();
        check(lexer != NULL);
        check(lexer->cur_pos == 0);
        check(lexer->cur_line == 0);
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
    }

    //it("Scans through the ROM file and collects each token")
    //{

    //}

}
