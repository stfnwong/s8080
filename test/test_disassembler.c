/*
 * TEST_DISASSEMBLER
 * Unit tests for Disassembler object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "disassem.h"
// testing framework
#include "bdd-for-c.h"

spec("Disassembler")
{
    it("Initializes correctly")
    {
        Disassembler* dis = disassembler_create();
        check(dis != NULL);
        check(dis->pc == 0);
        check(dis->codebuffer == 0);

        disassembler_destroy(dis);
    }
}
