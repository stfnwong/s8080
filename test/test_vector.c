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


spec("ByteVector")
{
    it("Should init correctly")
    {
        int test_capacity = 32;
        ByteVector* test_vec;

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
        ByteVector* test_vec;

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
        ByteVector* test_vec;

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

        // So far we have just pushed one element on at a 
        // time. We can actually push multiple elements at once.

        // before this call the capacity of the vector is 8
        // since 32 > (8 * 2) we grow the vector to (2 * capacity) + len = 16 + 32 = 48
        vector_push_back(test_vec, test_data + 4, 32);
        check(test_vec->size == 36);
        check(test_vec->capacity == 48);

        // Adding another 32 elements will 'only' cause the vector to 
        // double to 92 elements
        vector_push_back(test_vec, test_data + 36, 32);
        check(test_vec->size == 68);    // 32 + 36
        check(test_vec->capacity == 96);

        // If everything went well we should also be able to iterate over
        // the elements in the vector and check them
        for(int i = 0; i < test_vec->size; ++i)
            check(test_data[i] == vector_get_val(test_vec, i));

        vector_destroy(test_vec);
        free(test_data);
    }
}
