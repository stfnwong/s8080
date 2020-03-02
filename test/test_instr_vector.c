/*
 * TEST_INSTR_VECTOR
 * Unit test for instruction vector class
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "instr.h"
#include "vector.h"
// testing framework
#include "bdd-for-c.h"


spec("InstrVector")
{
    it("Should init correctly")
    {
        int test_capacity = 32;
        InstrVector* test_vec;

        test_vec = instr_vector_create(test_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == test_capacity);

        instr_vector_destroy(test_vec);
    }

    it("Should allow access by reference")
    {
        int test_capacity = 32;
        InstrVector* test_vec;

        test_vec = instr_vector_create(test_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == test_capacity);

        Instr test_instr;
        for(int i = 0; i < test_capacity; ++i)
        {
            test_instr.instr = (uint8_t) (i+1) % 256;
            test_instr.addr  = 0xBEEF + i;
            test_instr.size  = 1;

            instr_vector_push_back(test_vec, &test_instr);
            check(test_vec->size == i+1);
        }
        check(test_vec->size == test_capacity);

        // Now check each instruction
        Instr* out_instr;
        for(int i = 0; i < test_vec->size; ++i)
        {
            out_instr = instr_vector_get(test_vec, i);
            check(out_instr != NULL);
            check(out_instr->instr == (i+1) % 256);
            check(out_instr->addr == 0xBEEF + i);
            check(out_instr->size == 1);
        }

        out_instr = instr_vector_get(test_vec, -1);
        check(out_instr == NULL);
        out_instr = instr_vector_get(test_vec, test_vec->size+1);
        check(out_instr == NULL);

        instr_vector_destroy(test_vec);
    }

    it("Should expand when full")
    {
        int start_capacity = 2;
        int total_test_size = 64;
        InstrVector* test_vec;

        test_vec = instr_vector_create(start_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == start_capacity);

        // Create some instructions and place them in the vector
        Instr test_instr; 

        test_instr.instr = 0x1;
        test_instr.addr  = 0xBEEF;
        test_instr.size  = 1;

        // The intial capacity is 2, so pushing back a 
        // single instruction won't trigger a re-alloc
        instr_vector_push_back(test_vec, &test_instr);
        check(test_vec->size == 1);
        check(test_vec->capacity == 2);
        
        // The test for re-alloc is when size >= capacity, 
        // but since size isnt updated until exit the vector
        // won't expand until the next instruction is pushed
        test_instr.instr = 0x2;
        test_instr.addr  = 0xBEEF + 1;
        test_instr.size  = 1;
        instr_vector_push_back(test_vec, &test_instr);
        check(test_vec->size == 2);
        check(test_vec->capacity == 2);

        // Pushing another instruction will trigger a 
        // capacity epansion to 2 * vec->capacity
        test_instr.instr = 0x2;
        test_instr.addr  = 0xBEEF + 1;
        test_instr.size  = 1;
        instr_vector_push_back(test_vec, &test_instr);
        check(test_vec->size == 3);
        check(test_vec->capacity == 4);

        // Now if we push 4 more instructions in, the size will 
        // be 7 and the capcity will expand to 8 at the 4th instruction
        for(int i = 0; i < 4; ++i)
        {
            test_instr.instr = 0x3 + i;
            test_instr.addr  = 0xBEEF + 2 + i;
            test_instr.size  = 1;
            instr_vector_push_back(test_vec, &test_instr);
            check(test_vec->size == i + 3 + 1);     // +1 to account for zero index start
            if(i >= 1)
                check(test_vec->capacity == 8);
            if(i < 1)
                check(test_vec->capacity == 4);
        }
        check(test_vec->size == 7);
        check(test_vec->capacity == 8);

        instr_vector_destroy(test_vec);
    }
}
