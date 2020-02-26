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

    it("Should expand when full")
    {
        int start_capacity = 2;
        int total_test_size = 256;
        Vector* test_vec;

        test_vec = vector_create(start_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == start_capacity);

        // generate test data
        uint8_t* test_data = malloc(sizeof(uint8_t) * total_test_size);
        check(test_data != NULL);

        // Since the capacity is two, after we insert 2 elements we
        // expect the capacity to increase to 4
        vector_push_back(test_vec, test_data, 1);
        check(test_vec->size == 1);
        check(test_vec->capacity == 2);

        vector_push_back(test_vec, test_data + 1, 1);
        check(test_vec->size == 2);
        check(test_vec->capacity == 4);

        vector_push_back(test_vec, test_data + 2, 1);
        check(test_vec->size == 3);
        check(test_vec->capacity == 4);

        vector_push_back(test_vec, test_data + 3, 1);
        check(test_vec->size == 4);
        check(test_vec->capacity == 8);

        //vector_push_back(test_vec, test_data + 4, 32);
        //check(test_vec->size == 36);
        //check(test_vec->capacity == 128);

        // Memory gets freed along with vector
        vector_destroy(test_vec);
    }
}
