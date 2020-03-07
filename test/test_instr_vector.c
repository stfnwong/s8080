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

    it("Should preserve contents correctly when expanded")
    {
        int start_capacity = 1;
        int total_test_size = 64;
        InstrVector* test_vec;

        test_vec = instr_vector_create(start_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == start_capacity);

        test_vec = byte_vector_create(start_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == start_capacity);

        // Pre-compute the expected outputs 
        uint16_t* exp_addrs = malloc(sizeof(uint16_t) * total_test_size);
        check(exp_addrs != NULL);

        uint32_t* exp_instrs = malloc(sizeof(uint32_t) * total_test_size);
        check(exp_instrs != NULL);

        for(int i = 0; i < total_test_size; ++i)
        {
            exp_addrs[i] = 0xBEEF + i;
            exp_instrs[i] = i+1;
        }

        // Input instructions
        Instr test_instr;
        Instr* out_instr;
        // set the initial addr and instr
        test_instr.addr  = exp_addrs[0];
        test_instr.instr = exp_instrs[0];
        test_instr.size  = 1;

        // Since the initial capacity is just one, we will get an expansion
        // after the second insert, which will also invoke a copy
        instr_vector_push_back(test_vec, &test_instr);
        check(test_vec->size == 1);
        check(test_vec->capacity == 1);
        // Also we should be able to read these back 
        // and check the values
        out_instr = instr_vector_get(test_vec, 0);
        check(out_instr != NULL);
        check(out_instr->addr == test_instr.addr);
        check(out_instr->instr == test_instr.instr);
        // Note also that out of range accesses will return NULL instructions 
        out_instr = instr_vector_get(test_vec, 20);
        check(out_instr == NULL);

        // Next instruction will cause an expansion
        test_instr.addr = exp_addrs[1];
        test_instr.instr = exp_instrs[1];
        instr_vector_push_back(test_vec, &test_instr);
        check(test_vec->size == 2);
        check(test_vec->capacity == 2);

        // Check all the previous instructions 
        for(int v = 0; v < test_vec->size; ++v)
        {
            out_instr = instr_vector_get(test_vec, v);
            check(out_instr != NULL);
            check(out_instr->instr == exp_instrs[v]);
            check(out_instr->addr  == exp_addrs[v]);
        }
        fprintf(stdout, "[%s] test_vec at size %d\n", __func__, test_vec->size);
        instr_vector_print(test_vec);
        fprintf(stdout, "\n");

        // Lets add some more vectors to the list up to 16
        for(int v = 2; v < 16; ++v)
        {
            test_instr.instr = exp_instrs[v];
            test_instr.addr = exp_addrs[v];
            instr_vector_push_back(test_vec, &test_instr);
            check(test_vec->size == v+1);
        }
        fprintf(stdout, "[%s] test_vec at size %d\n", __func__, test_vec->size);
        instr_vector_print(test_vec);
        fprintf(stdout, "\n");

        // These will also have been copied across to the 'new' buffer
        for(int v = 2; v < 16; ++v)
        {
            out_instr = instr_vector_get(test_vec, v);
            check(out_instr != NULL);
            check(out_instr->instr == exp_instrs[v]);
            check(out_instr->addr == exp_addrs[v]);
        }
        fprintf(stdout, "[%s] test_vec at size %d\n", __func__, test_vec->size);
        instr_vector_print(test_vec);
        fprintf(stdout, "\n");

        for(int v = 16; v < total_test_size; ++v)
        {
            test_instr.instr = exp_instrs[v];
            test_instr.addr = exp_addrs[v];
            instr_vector_push_back(test_vec, &test_instr);
            check(test_vec->size == v+1);
        }

        // Continue this until the end of the test
        for(int v = 16; v < total_test_size; ++v)
        {
            out_instr = instr_vector_get(test_vec, v);
            check(out_instr != NULL);
            check(out_instr->instr == exp_instrs[v]);
            check(out_instr->addr == exp_addrs[v]);
        }
        fprintf(stdout, "[%s] test_vec at size %d\n", __func__, test_vec->size);
        instr_vector_print(test_vec);
        fprintf(stdout, "\n");

        byte_vector_destroy(test_vec);
        free(exp_addrs);
        free(exp_instrs);
    }
}
