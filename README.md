# 8080 Emulator

Its an emulator for the Intel 8080 CPU. Its also an assembler and disassembler. Like very many people who have written emulators for this architecture, I started by following along with the emulator at [Emulator 101](http://www.emulator101.com/full-8080-emulation.html).

### Requirements 
This is basically self contained. Tests are done with [bdd-for-c](https://github.com/grassator/bdd-for-c). The header file is included in the test directory. `bdd-for-c` requires libncurses 5.x and libbsd. I compile with gcc 9.2.1 but any C99 compiler should be fine.
