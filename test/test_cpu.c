/*
 * TEST_CPU
 * Unit test for CPU object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <string.h>
#include "cpu.h"
// testing framework
#include "bdd-for-c.h"

spec("CPU")
{
    // NOTE: Are we going to have tests for each instruction individually?
    it("Should create and destroy state objects correctly")
    {
        CPUState* test_state;

        test_state = cpu_create();
        // Check the object was allocated 
        check(test_state != NULL);
        // Check the memory was allocated 
        check(test_state->memory != NULL);
        // NOTE: because the memory is a fixed size we don't really need to 
        // pass this parameter around as part of the struct.
        //check(test_state->mem_size == CPU_MEM_SIZE);  
        // Memory is not zeroed by defaul, 
        // but contents of state pointer is 
        // registers
        check(test_state->a == 0);
        check(test_state->b == 0);
        check(test_state->c == 0);
        check(test_state->d == 0);
        check(test_state->e == 0);
        check(test_state->h == 0);
        check(test_state->l == 0);
        check(test_state->sp == 0);
        check(test_state->pc == 0);
        // condition codes
        check(test_state->cc.z == 0);

        cpu_destroy(test_state);
    }
}
