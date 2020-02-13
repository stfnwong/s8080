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
        check(symbol_table_empty(test_table) == 1);
        check(symbol_table_full(test_table) == 0);

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
        out_sym = symbol_table_get_idx(test_table, -1);
        check(out_sym == NULL);
        out_sym = symbol_table_get_idx(test_table, 5);
        check(out_sym == NULL);
        // get a real symbol
        out_sym = symbol_table_get_idx(test_table, 0);
        check(out_sym != NULL);
        check(out_sym->addr == 0xBEEF);
        check(strncmp(out_sym->sym, "TEST\0", 5) == 0);

        out_sym = symbol_table_get_idx(test_table, 1);
        check(out_sym != NULL);
        check(out_sym->addr == 0xBEEF + 1);
        check(strncmp(out_sym->sym, "TEST\0", 5) == 0);

        symbol_table_destroy(test_table);
    }

    it("Should allow lookup by string search")
    {
        SymbolTable* test_table;
        int table_size = 32;
        int num_syms = 10;
        char label_str[32];

        // create the table
        test_table = symbol_table_create(table_size);
        check(test_table != NULL);

        Symbol  test_sym;
        Symbol* out_sym;

        strncpy(label_str, "TEST_0\0", 7);
        for(int s = 0; s < num_syms; ++s)
        {
            test_sym.addr = 0xD0D0 + s;
            strncpy(&test_sym.sym, label_str, 7);
            symbol_table_add_sym(test_table, &test_sym);
            label_str[5] += 1;      // TODO  : should work since this is ASCII, but not robust
            check(test_table->size == s + 1);
        }
        check(test_table->size == num_syms);

        // Now start looking up symbols by string
        out_sym = symbol_table_get_str(test_table, "INVALID_STR", 11);
        check(out_sym == NULL);
        out_sym = symbol_table_get_str(test_table, "TEST_0", 6);
        check(out_sym != NULL);
        out_sym = symbol_table_get_str(test_table, "TEST_1", 6);
        check(out_sym != NULL);
        out_sym = symbol_table_get_str(test_table, "TEST_9", 6);
        check(out_sym != NULL);


        symbol_table_destroy(test_table);
    }
}
