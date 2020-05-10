#!/bin/bash
# 
# RUN_TESTS
# Run all the tests

make clean && make all
./bin/test/test_assembler
./bin/test/test_instr_buffer
./bin/test/test_instr_vector
./bin/test/test_lexer
./bin/test/test_line_info
./bin/test/test_list
./bin/test/test_opcode
./bin/test/test_source_info
./bin/test/test_symbol_table
./bin/test/test_token
./bin/test/test_vector
