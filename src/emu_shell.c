/* EMULATOR SHELL
 * As per http://www.emulator101.com/emulator-shell.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "emu_shell.h"


// Simple parity loop. Probably can replace this with a faster routine later 
inline uint8_t Parity(uint8_t inp)
{
    uint8_t n = 0;
    while(inp)
    {
        ++n;
        inp &= inp - 1;
    }

    return n;
}

// Common instructions in arithmetic group
inline void arith_set_flags(State8080 *state, uint16_t ans)
{
    state->cc.z = ((ans & 0xFF) == 0);
    // Sign flag: if bit 7 is set then set the sign flag
    state->cc.s =  ((ans & 0x80) != 0);
    // Carry flag
    state->cc.cy = ((ans > 0xff) != 0);
    // Handle parity in a subroutine 
    state->cc.p = Parity(ans & 0xFF);
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

        // ======== ARITHMETIC GROUP ======== //
        case 0x80:      // ADD B
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->b;
                // Zero flag: if the result is zero 
                // set the zero flag, otherwise clear it
                state->cc.z = ((ans & 0xFF) == 0);
                // Sign flag: if bit 7 is set then set the 
                // sign flag
                state->cc.s =  ((ans & 0x80) != 0);
                // Carry flag
                state->cc.cy = ((ans > 0xff) != 0);
                // Handle parity in a subroutine 
                state->cc.p = Parity(ans & 0xFF);
                state->a = ans & 0xFF;
            }
        case 0x81:  // ADD C 
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->c;
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }
        case 0x82:  // ADD D
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->d;
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }
        case 0x83:  // ADD E
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->e;
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }
        case 0x84:      // ADD H
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->h;
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }


        case 0x86:      // ADD M    (memory form)
            {
                uint16_t offset = (state->h << 8) | (state->l);
                uint16_t ans = (uint16_t) state->a + state->memory[offset];
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }


        case 0xC6:      // ADD ADI (immediate form)
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) opcode[1];
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }



    }
    state->pc += 1;     

    return 0;       // TODO : what is the correct thing to return here?
}
