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
        Vector* test_vec;
        int test_capacity = 32;

        test_vec = vector_create(test_capacity);
        check(test_vec != NULL);
        check(test_vec->size == 0);
        check(test_vec->capacity == test_capacity);

        vector_destroy(test_vec);
    }
}
