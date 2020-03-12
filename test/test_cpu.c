/*
 * TEST_CPU
 * Unit test for CPU object
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include "cpu.h"
// testing framework
#include "bdd-for-c.h"


// Implements the CPM style print function
static uint8_t test_port_in(void* cpu, uint8_t port)
{
    CPUState* state = (CPUState*) cpu;

    // Print characters in register E
    if(state->e == 0x2)
    {
        fprintf(stdout, "%c", state->e);
    }
    // Print from memory at DE until 'S' char
    else if(state->e == 0x9)
    {
        uint16_t addr = (state->d << 8) | state->e;
        do
        {
            fprintf(stdout, "%c", cpu_read_mem(state, addr));
            addr++;
        }while(cpu_read_mem(state, addr) != '$');
    }

    return 0xFF;
}

static int test_finished = 0;
static void test_port_out(void* cpu, uint8_t port, uint8_t data)
{
    test_finished = 1;
}



spec("CPU")
{
    // NOTE: Are we going to have tests for each instruction individually?
    it("Should create and destroy state objects correctly")
    {
        CPUState* test_cpu;

        test_cpu = cpu_create();
        // Check the object was allocated 
        check(test_cpu != NULL);
        // Check the memory was allocated 
        check(test_cpu->memory != NULL);
        check(test_cpu->inport != NULL);
        check(test_cpu->outport != NULL);
        // NOTE: because the memory is a fixed size we don't really need to 
        // pass this parameter around as part of the struct.
        //check(test_cpu->mem_size == CPU_MEM_SIZE);  
        // Memory is not zeroed by defaul, 
        // but contents of state pointer is 
        // registers
        check(test_cpu->a == 0);
        check(test_cpu->b == 0);
        check(test_cpu->c == 0);
        check(test_cpu->d == 0);
        check(test_cpu->e == 0);
        check(test_cpu->h == 0);
        check(test_cpu->l == 0);
        check(test_cpu->sp == 0);
        check(test_cpu->pc == 0);
        // condition codes
        check(test_cpu->cc.z == 0);

        cpu_destroy(test_cpu);
    }

    it("Should print info from the CPU tests")
    {
        // TODO : this needs to be assembled
        const char* test_file = "asm/cpu_test.asm\0";
        int test_offset = 0x100;
        int status = 0;
        CPUState* test_cpu;

        test_cpu = cpu_create();
        // Check the object was allocated 
        check(test_cpu != NULL);
        // Check the memory was allocated 
        check(test_cpu->memory != NULL);
        check(test_cpu->inport != NULL);
        check(test_cpu->outport != NULL);

        // use the I/O functions in this file for the test
        test_cpu->inport  = test_port_in;
        test_cpu->outport = test_port_out;

        status = cpu_load_memory(test_cpu, test_file, test_offset);
        check(status == 0);
        fprintf(stdout, "[%s] read file %s into memory...\n", __func__, test_file);

        // Artificially set PC
        test_cpu->pc = 0x100;
        // Set OUT 1, A at 0x0000 to stop the test
        cpu_write_mem(test_cpu, 0x0000, 0xD3);
        cpu_write_mem(test_cpu, 0x0001, 0x00);

        // Set IN. A, 0 at 0x0005 to output characters 
        cpu_write_mem(test_cpu, 0x0005, 0xDB);
        cpu_write_mem(test_cpu, 0x0006, 0x00);
        cpu_write_mem(test_cpu, 0x0007, 0xC9);

        fprintf(stdout, "[%s] set up CPU state for test :\n", __func__);
        cpu_print_state(test_cpu);

        fprintf(stdout, "[%s] first 0x100 bytes of memory: \n", __func__);
        cpu_print_memory(test_cpu, 0x100, 0x100);
        fprintf(stdout, "\n");

        long instr_count = 0;
        long cycle_count = 0;
        long c;

        test_finished = 0;
        while(!test_finished)
        {
            instr_count++;
            c = cpu_exec(test_cpu);
            cycle_count += c;
            cpu_print_state(test_cpu);
            if(instr_count > 64)
                break;
            //if(instr_count % 64 == 0)
            //    cpu_print_state(test_cpu);
        }
    }
}
