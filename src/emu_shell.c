/* EMULATOR SHELL
 * As per http://www.emulator101.com/emulator-shell.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "emu_shell.h"
#include "disassem/disassem.h"


// Simple parity loop. Probably can replace this with a faster routine later 
static inline uint8_t Parity(uint8_t inp)
{
    uint8_t n = 0;
    while(inp)
    {
        ++n;
        inp &= inp - 1;
    }

    return n;
}

static inline int Parity2(int x, int size)
{
    int i, p = 0;

    x = (x && ((1 << size)-1));
    for(i = 0; i < size; i++)
    {
        if(x & 0x01)
            p++;
        x = x >> 1;
    }

    return (0 == (p & 0x1));
}

// Common instructions in arithmetic group
static inline void arith_set_flags(State8080 *state, uint16_t ans)
{
    state->cc.z  = ((ans & 0xFF) == 0);
    // Sign flag: if bit 7 is set then set the sign flag
    state->cc.s  = ((ans & 0x80) != 0);
    // Carry flag
    state->cc.cy = ((ans > 0xff) != 0);
    // Handle parity in a subroutine 
    state->cc.p = Parity(ans & 0xFF);
} 

static inline void logic_set_flags(State8080 *state)
{
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.s = (0x80 == (state->a & 0x80));
    state->cc.p = Parity(state->a);
}


// Trap unimplemented instructions 
void UnimplementedInstruction(State8080 *state, unsigned char opcode)
{
    // PC will have advanced by one, so undo that 
    state->pc--;
    fprintf(stderr, "Unimplemented instruction 0x%02X\n", opcode);
    fprintf(stderr, "PC   INSTR\n");
    disassemble_8080_op(state->memory, state->pc);
    //exit(1);
}

int Emulate8080(State8080 *state)
{
    unsigned char *opcode;

    opcode = &state->memory[state->pc];
    switch(*opcode)
    {
        case 0x00:      // NOP
            break;
        case 0x01:
            UnimplementedInstruction(state, opcode[0]);
            return -1;
        case 0x02:
            UnimplementedInstruction(state, opcode[0]);
            return -1;
        case 0x03:
            UnimplementedInstruction(state, opcode[0]);
            return -1;

        case 0x07:      // RLC
            {
                uint8_t a_prev = state->a;
                state->a = (state->a << 1) | (a_prev & 0x80);
                state->cc.cy = (a_prev & 0x80);         // TODO: fix overflow here
            }
        case 0x08:
            break;

        case 0x0C:      // INCR C
            {
                state->c += 1;
                // TODO : set flags....
            }
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
                arith_set_flags(state, ans);
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
        case 0x85:      // ADD L
            {


            }
        case 0x86:      // ADD M    (memory form)
            {
                uint16_t offset = (state->h << 8) | (state->l);
                uint16_t ans = (uint16_t) state->a + state->memory[offset];
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }
        case 0x87:      // ADD A
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->a;
                arith_set_flags(state, ans);
                state->a = ans & 0xFF;
            }
        case 0x88:      // ADC B  (A <- A + B + CY)
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) state->b;
                arith_set_flags(state, ans);
                state->a = (ans + state->cc.cy) & 0xFF;
            }

        case 0xA7:      // ANA A
            {
                state->a = state->a & state->a;
                logic_set_flags(state);
            }
            break;

        case 0xC1:      // POP B
            {
                state->c = state->memory[state->sp];
                state->b = state->memory[state->sp+1];
                state->sp += 2;
            }
            break;

        case 0xC2:      // JNZ ADR
            {
                if(state->cc.z == 0)
                    state->pc = (opcode[2] << 8) | opcode[1];
                else
                    state->pc += 2;
            }
            break;
        case 0xC3:      // JMP ADR
            {
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            break;

        case 0xC5:     // PUSH B
            {
                state->memory[state->sp-1] = state->b;
                state->memory[state->sp]   = state->c;
                state->sp -= 2;
            }
            break;

        case 0xC6:      // ADD ADI (immediate form)
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) opcode[1];
                arith_set_flags(state, ans);
            }


        case 0xCD:      // CALL ADR
            {
                // save reutrn address
                uint16_t ret = state->pc+2;
                state->memory[state->sp-1] = (ret >> 8) & 0xFF;
                state->memory[state->sp-2] = ret & 0xFF;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            break;

        case 0xD1:      // POP D
            {
                state->e = state->memory[state->sp];
                state->d = state->memory[state->sp-1];
                state->sp += 2;
            }
            break;

        case 0xD4:      // CNC (if NCY, CALL adr)
            {
                UnimplementedInstruction(state, opcode[0]);
                return -1;
            }
            break;

        case 0xD5:      // PUSH D
            {

                state->memory[state->sp-1] = state->d;
                state->memory[state->sp-2] = state->e;
                state->sp -= 2;
            }
            break;


        default:
            UnimplementedInstruction(state, opcode[0]);
            return -1;




    }
    state->pc += 1;     

    return 0;       // TODO : what is the correct thing to return here?
}
