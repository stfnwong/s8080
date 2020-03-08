# S8080 Emulator

[![Build Status](https://travis-ci.org/stfnwong/s8080.svg?branch=master)](https://travis-ci.org/stfnwong/s8080)

Its an emulator for the Intel 8080 CPU. Its also an assembler and disassembler (in progress). At some point there might be some HDL.

### Requirements 
This is basically self contained. Tests are done with [bdd-for-c](https://github.com/grassator/bdd-for-c). The header file is included in the test directory. `bdd-for-c` requires `libncurses 5.x` and `libbsd`.
