/*
 * TEST_VECTOR
 * Unit test for vector (extendable array) class
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "vector.h"
// testing framework
#include "bdd-for-c.h"


spec("Vector")
{
    it("Should init correctly")
    {
        int test_capacity = 32;
        Vector* test_vec;

        test_vec = vector_create(test_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == test_capacity);

        vector_destroy(test_vec);
    }

    it("Should allow access to references and values")
    {
        int test_capacity = 8;
        int total_test_size = 16;
        Vector* test_vec;

        test_vec = vector_create(test_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == test_capacity);

        // generate test data
        uint8_t* test_data = malloc(sizeof(uint8_t) * total_test_size);
        check(test_data != NULL);
        // Init the test data 
        for(int i = 0; i < total_test_size; ++i)
            test_data[i] = (i+1) % 256;

        // Don't worry about the growth rate here
        // Would also be nice if I could pass test_data[i] instead of (test_data + i)
        for(int i = 0; i < total_test_size; ++i)
            vector_push_back(test_vec, test_data + i, 1);
        check(test_vec->size == total_test_size);

        // Check that we can access by value
        for(int i = 0; i < total_test_size; ++i)
        {
            uint8_t val = vector_get_val(test_vec, i);
            check(val == test_data[i]);
        }
        // Check that we can access by reference
        for(int i = 0; i < total_test_size; ++i)
        {
            uint8_t* ref = vector_get(test_vec, i);
            check(*ref == test_data[i]);
        }

        vector_destroy(test_vec);
        free(test_data);
    }

    it("Should expand when full")
    {
        int start_capacity = 2;
        int total_test_size = 64;
        uint8_t* val;
        Vector* test_vec;

        test_vec = vector_create(start_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == start_capacity);

        // generate test data
        uint8_t* test_data = malloc(sizeof(uint8_t) * total_test_size);
        check(test_data != NULL);
        // Init the test data 
        for(int i = 0; i < total_test_size; ++i)
            test_data[i] = (i+1) % 256;

        // Since the capacity is two, after we insert 2 elements we
        // expect the capacity to increase to 4
        vector_push_back(test_vec, test_data, 1);
        check(test_vec->size == 1);
        check(test_vec->capacity == 2);
        val = vector_get(test_vec, 0);
        check(*val == 1);   // (0+1) % 256

        vector_push_back(test_vec, test_data + 1, 1);
        check(test_vec->size == 2);
        check(test_vec->capacity == 4);
        val = vector_get(test_vec, 1);
        check(*val == 2)    // (1 + 1) % 256

        vector_push_back(test_vec, test_data + 2, 1);
        check(test_vec->size == 3);
        check(test_vec->capacity == 4);
        val = vector_get(test_vec, 2);
        check(*val == 3);   // (2 + 1) % 256

        vector_push_back(test_vec, test_data + 3, 1);
        check(test_vec->size == 4);
        check(test_vec->capacity == 8);

        // TODO : test is fine when we push back one or two elements 
        // but fails if we try to extend by a large amount
        //vector_push_back(test_vec, test_data + 4, 32);
        //check(test_vec->size == 36);
        //check(test_vec->capacity == 128);

        vector_destroy(test_vec);
        free(test_data);
    }
}
