/* EMULATOR SHELL
 * As per http://www.emulator101.com/emulator-shell.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "emu_shell.h"


uint8_t Parity(uint8_t inp)
{


}

// Trap unimplemented instructions 
void UnimplementedInstruction(State8080 *state)
{
    // PC will have advanced by one, so undo that 
    fprintf(stderr, "Unimplemented instruction\n");
    exit(1);
}

int Emulate8080(State8080 *state)
{
    unsigned char *opcode;

    opcode = &state->memory[state->pc];
    switch(*opcode)
    {
        case 0x00:
            UnimplementedInstruction(state);
            break;
        case 0x01:
            UnimplementedInstruction(state);
            break;
        case 0x02:
            UnimplementedInstruction(state);
            break;
        case 0x03:
            UnimplementedInstruction(state);
            break;
        // Implement a few and see
        case 0x41:      // MOV B, C
            state->b = state->c;
            break;
        case 0x42:      // MOV B,D
            state->b = state->c;
            break;
        case 0x43:      // MOV B,E
            state->b = state->e;
            break;
        case 0x44:      // MOV B,L
            state->b = state->l;
            break;
        case 0x45:      // MOV B,M
            // TODO : Need to deference HL
            //state->b = state->m;
            break;

            // TODO ... more instructions 
        case 0x80:      // ADD B
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->b;
                // Zero flag: if the result is zero 
                // set the zero flag, otherwise clear it
                state->cc.z = ((ans & 0xFF) == 0);
                // Sign flag: if bit 7 is set then set the 
                // sign flag
                state->cc.s = ((ans & 0x80) != 0);
                // Carry flag
                state->cc.cy = (ans > 0xff != 0);
                // Handle parity in a subroutine 
                state->cc.p = Parity(ans & 0xFF);
            }


    }
    state->pc += 1;     
}
