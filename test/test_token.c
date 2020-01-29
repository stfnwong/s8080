/*
 * TEST_TOKEN
 * Unit tests for token object
 *
 * Stefan Wong 2020
 */


#include <stdio.h>
#include "assembler.h"
// testing framework
#include "bdd-for-c.h"

// NOTE: can we have two specs in the same file?
spec("Token")
{
    it("Inits with type SYM_NONE and null string")
    {
        Token* token = create_token();
        check(token != NULL);

        check(token->type == SYM_NONE);
        check(token->token_str[0] == '\0');

        destroy_token(token);
    }
}
