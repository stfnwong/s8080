/*
 * TEST_SYMBOL_TABLE
 * Unit tests for SymbolTable object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "lexer.h"
// testing framework
#include "bdd-for-c.h"


spec("SymbolTable")
{

    it("Should allow insertion")
    {
        SymbolTable* test_table;
        int table_size = 32;

        test_table = symbol_table_create(table_size);
        check(test_table != NULL);
        check(test_table->max_size == table_size);
        check(test_table->size == 0);

        // Put some symbols in the table
        Symbol test_sym;
        Symbol* out_sym;

        test_sym.addr = 0xBEEF;
        strncpy(&test_sym.sym, "TEST\0", 5);
        symbol_print(&test_sym);

        symbol_table_add_sym(test_table, &test_sym);
        check(test_table->size == 1);
        test_sym.addr++;
        symbol_table_add_sym(test_table, &test_sym);
        check(test_table->size == 2);

        // Check the symbols 
        out_sym = symbol_table_get(test_table, -1);
        check(out_sym == NULL);
        out_sym = symbol_table_get(test_table, 5);
        check(out_sym == NULL);
        // get a real symbol
        out_sym = symbol_table_get(test_table, 0);


        // TODO : something wrong with free() here
        symbol_table_destroy(test_table);
    }
}
