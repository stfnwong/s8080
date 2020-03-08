/*
 * TEST_INSTR_BUFFER
 * Unit tests for InstrBuffer object
 *
 * Stefan Wong 2020
 */


#include <stdio.h>
#include <string.h>
#include "instr.h"
// testing framework
#include "bdd-for-c.h"

spec("IntrBuffer")
{
    it("Should create and destroy correctly")
    {
        int buf_size = 32;
        InstrBuffer* test_buffer;

        test_buffer = instr_buffer_create(buf_size);
        check(test_buffer != NULL);
        check(test_buffer->max_size == buf_size);
        check(test_buffer->size == 0);

        instr_buffer_destroy(test_buffer);
    }

    it("Should destroy correctly even when not full")
    {
        int buf_size = 32;
        InstrBuffer* test_buffer;

        test_buffer = instr_buffer_create(buf_size);
        check(test_buffer != NULL);
        check(test_buffer->max_size == buf_size);
        check(test_buffer->size == 0);

        // Create some instructions
        for(int i = 0; i < (buf_size >> 1); ++i)
        {
            Instr ins;
            ins.instr = i;
            ins.addr = 0xBEEF + i;
            instr_buffer_insert(test_buffer, &ins);
            check(test_buffer->size == i+1);
        }
        check(test_buffer->size == (buf_size >> 1));
        // Check the instructions before we delete
        for(int i = 0; i < (buf_size >> 1); ++i)
        {
            Instr* check_instr = instr_buffer_get(
                    test_buffer,
                    i
            );
            check(check_instr != NULL);
            check(check_instr->addr == 0xBEEF + i);
            check(check_instr->instr == i);
        }

        instr_buffer_destroy(test_buffer);
    }

    it("Should allow insert")
    {
        int buf_size = 32;
        InstrBuffer* test_buffer;

        test_buffer = instr_buffer_create(buf_size);
        check(test_buffer != NULL);
        check(test_buffer->max_size == buf_size);
        check(test_buffer->size == 0);

        // Create some instructions
        for(int i = 0; i < buf_size-1; ++i)
        {
            Instr ins;
            ins.instr = i;
            ins.addr = 0xBEEF + i;
            instr_buffer_insert(test_buffer, &ins);
            check(test_buffer->size == i+1);
        }
        check(test_buffer->size == buf_size-1);

        // if we try to get elements outside the range we
        // should get a NULL back
        Instr* check_instr;

        check_instr = instr_buffer_get(test_buffer, -1);
        check(check_instr == NULL);
        check_instr = instr_buffer_get(test_buffer, buf_size + 2);
        check(check_instr == NULL);
        // Since we added one less than max, max will also be null
        check_instr = instr_buffer_get(test_buffer, buf_size);
        check(check_instr != NULL);
        check_instr = instr_buffer_get(test_buffer, buf_size - 2);
        check(check_instr != NULL);
        check(check_instr->addr == 0xBEEF + buf_size - 2);
        check(check_instr->instr == buf_size-2);

        // Check instructions
        for(int i = 0; i < buf_size-1; ++i)
        {
            check_instr = instr_buffer_get(
                    test_buffer,
                    i
            );
            check(check_instr != NULL);
            check(check_instr->addr == 0xBEEF + i);
            check(check_instr->instr == i);
        }

        instr_buffer_destroy(test_buffer);
    }
}
