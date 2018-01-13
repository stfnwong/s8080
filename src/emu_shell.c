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
    uint8_t x;

    x = inp;
    while(x)
    {
        ++n;
        x &= x - 1;
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


// ==== Setup initial state
State8080 *initState(void)
{
    State8080 *state = calloc(1, sizeof(State8080));
    if(!state)
        return NULL;
    state->memory = malloc(0x10000);        // 16K
    if(!state->memory)
        return NULL;

    return state;
}

void freeState(State8080 *state)
{
    free(state->memory);
    free(state);
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
        case 0x05:      // DCR B
            {
                uint8_t res = state->b - 1;
                arith_set_flags(state, res);
                state->b = res;
            }
            break;
        case 0x06:      // MVI B, d8
            {
                state->b = opcode[1];
                state->pc++;
            }
            break;

        case 0x07:      // RLC
            {
                uint8_t a_prev = state->a;
                state->a = (a_prev << 1) | (a_prev & 0x80);
                state->cc.cy = (a_prev >> 7) ? 1 : 0;
            }
        case 0x08:      // NOP
            break;

        case 0x09:      // DAD B
            {
                // HL = HL + BC 
                uint32_t hl, bc, res;
                hl = (state->h << 8) | state->l;
                bc = (state->b << 8) | state->c;
                res = hl + bc;
                state->h = (res & 0xFF00) >> 8;
                state->l = res & 0xFF;
                state->cc.cy = ((res & 0xFFFF0000) > 0);
            }
            break;
        case 0x0C:      // INCR C
            {
                uint16_t res = state->c + 1;
                arith_set_flags(state, res);
                state->c = res;
            }
            break;
        case 0x0D:      // DCR C
            {
                uint16_t res = state->c - 1;
                arith_set_flags(state, res);
                state->c = res;
            }
            break;
        case 0x0E:      // MVI, C, #d8
            {
                state->c = opcode[1];
            }
            break;
        case 0x0F:      // RRC 
            {
                uint8_t a_prev = state->a;
                state->a = a_prev >> 1;
                state->a = state->a | (a_prev & 0x01);
                state->cc.cy = (a_prev & 0x01) != 0;
            }
        case 0x10:      // NOP
            break;
        case 0x11:      // LXI, D,D16
            {
                state->d = opcode[2];
                state->e = opcode[1];
            }
            break;
        case 0x13:      // INX D
            {
                state->e++;
                if(state->e == 0)
                    state->d++;
                break;
            }

        case 0x19:      // DAD D
            {
                uint32_t hl, de, res;
                hl = (state->h << 8) | state->l;
                de = (state->d << 8) | state->e;
                res = hl + de;
                state->h = (res & 0xFF00) >> 8;
                state->l = res & 0xFF;
                state->cc.cy = ((res & 0xFFFF0000) != 0);
            }
            break;

        case 0x1A:      // LDAX D
            {
                uint16_t offset = (state->d << 8) | state->e;
                state->a = state->memory[offset];
            }
            break;
        case 0x1B:      // DCX D 
            {
                uint32_t de, res;
                de = (state->d << 8) | state->e;
                res = de - 1;
                state->d = (res & 0xFF00) >> 8;
                state->e = res & 0xFF;
                state->cc.cy = ((res & 0xFFFF0000) != 0);
            }
            break;

        case 0x21:      // LXI H, word
            {
                state->l = opcode[1];
                state->h = opcode[2];
                state->pc += 2;
            }
            break;

        case 0x23:      // INX H
            {
                state->l++;
                if(state->l == 0)
                    state->h++;
            }
            break;
        case 0x24:      // INR H
            {
                uint16_t hl;
                hl = (state->h << 8) | state->l;
                hl += 1;
                arith_set_flags(state, hl);
                state->h = (hl & 0xFF00) >> 8;
                state->l = hl & 0xFF;  
            }
            break;
        case 0x25:      // DCR H
            {
                uint16_t res = state->h - 1;
                arith_set_flags(state, res);
                state->h = res;
            }
            break;

        case 0x26:      // MVI, H, byte
            {
                state->h = opcode[1];
                state->pc++;
            }
            break;

        case 0x29:      // DAD H
            {
                uint32_t hl, res;
                hl = (state->h << 8) | state->l;
                res = hl + hl;
                state->h = (res & 0xFF00) >> 8;
                state->l = res & 0xFF;
                state->cc.cy = ((res & 0xFFFF0000) != 0);
            }
            break;

        case 0x2A:      // LHLD ADR
            {
                state->l = state->memory[opcode[1]];
                state->h = state->memory[opcode[1] + 1];
            }
            break;

        case 0x2B:      // DCX H
            {
                uint16_t hl;
                hl = (state->h << 8) | state->l;
                hl = hl + 1;
                state->h = (hl >> 8) & 0xFF;
                state->l = hl & 0xFF;
            }
            break;
        case 0x2C:      // INR L
            {
                uint16_t res = state->l + 1;
                arith_set_flags(state, res);
                state->l = res;
            }
            break;

        case 0x2D:      // DCR L 
            {
                uint16_t res = state->l - 1;
                arith_set_flags(state, res);
                state->l = res;
            }
            break;

        case 0x2E:      // MVI L, D8
            state->l = opcode[1];
            break;
        case 0x2F:      // CMA A
            state->a = ~state->a;
            break;

        case 0x31:      // LXI, SP, D16
            {
                uint16_t sp;
                sp = (opcode[3] << 8) | opcode[2];
                state->sp = sp;
            }
            break;

        case 0x32:      // STA, adr
            {
                state->memory[opcode[1]] = state->a;
            }
            break;

        case 0x33:      // INX SP
            state->sp += 1;
            break;

        case 0x34:      // INR M
            {
                // TODO  : the implementation of this may not be correct
                uint16_t hl; 
                hl = (state->h << 8) | state->l;
                hl += 1;
                arith_set_flags(state, hl);
                state->h = (hl >> 8) & 0xFF;
                state->l = 0xFF;
            }
            break;


        case 0x36:      // MVI, M byte
            {
                // AC set if lower half-byte was zero before decrement 
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->memory[offset] = opcode[1];
                state->pc++;
            }
            break;

        case 0x3A:      // LDA (word)
            {
                uint16_t offset;
                offset = (opcode[2] << 8) | opcode[1];
                state->a = state->memory[offset];
                state->pc += 2;
            }
            break;

        case 0x3B:      // DCX SP
            {
                state->sp -= 1;
            }
            break;

        case 0x3C:      // INR A
            {
                uint16_t res = state->a + 1;
                arith_set_flags(state, res);
                state->a = res;
            }
            break;

        case 0x3D:      // DCR A
            {
                uint16_t res = state->a - 1;
                arith_set_flags(state, res);
                state->a = res;
            }
            break;

        case 0x40:      // MOV B, B
            state->b = state->b;
            break;
        case 0x41:      // MOV B, C
            state->b = state->c;
            break;
        case 0x42:      // MOV B,D
            state->b = state->c;
            break;
        case 0x43:      // MOV B,E
            state->b = state->e;
            break;
        case 0x44:      // MOV B, H
            state->b = state->h;
            break;
        case 0x45:      // MOV B,L
            state->b = state->l;
            break;
        case 0x46:      // MOV B,M
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->b = state->memory[offset];
            }
            break;
        case 0x47:      // MOV B,A
            state->b = state->a;
            break;
        case 0x48:      // MOV C, B
            state->c = state->b;
            break;
        case 0x49:      // MOV C, C
            state->c = state->c;
            break;
        case 0x4A:      // MOV C, D
            state->c = state->d;
            break;
        case 0x4B:      // MOV C, E
            state->c = state->e;
            break;
        case 0x4C:      // MOV C, H
            state->c = state->h;
            break;
        case 0x4D:      // MOV C, L
            state->c = state->l;
            break;
        case 0x4E:      // MOV C, M
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->c = state->memory[offset];
            }
            break;
        case 0x4F:      // MOV C, A
            state->c = state->a;
            break;
        case 0x50:      // MOV D, B
            state->d = state->b;
            break;
        case 0x51:      // MOV, D, C
            state->d = state->c;
            break;
        case 0x52:      // MOV D, D
            state->d = state->d;
            break;
        case 0x53:      // MOV D,E
            state->d = state->e;
            break;
        case 0x54:      // MOV D H
            state->d = state->h;
            break;
        case 0x55:      // MOV D, L
            state->d = state->l;
            break;
        case 0x56:      // MOV D, M
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->d = state->memory[offset];
            }
            break;
        case 0x57:      // MOV D, A
            state->d = state->a;
            break;
        case 0x58:      // MOV E, B
            state->e = state->b;
            break;
        case 0x59:      // MOV E,C
            state->e = state->c;
            break;
        case 0x5A:      // MOV E,D
            state->e = state->d;
            break;
        case 0x5B:      // MOV E,E
            state->e = state->e;
            break;
        case 0x5C:      // MOV E, H
            state->e = state->h;
            break;
        case 0x5D:      // MOV E, L
            state->e = state->l;
            break;
        case 0x5E:      // MOV, E, H
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->e = state->memory[offset];
            }
            break;
        case 0x5F:      // MOV E,A
            state->e = state->a;
            break;
        case 0x60:      // MOV H,B
            state->h = state->b;
            break;
        case 0x61:      // MOV H, C
            state->h = state->c;
            break;
        case 0x62:      // MOV H, D
            state->h = state->d;
            break;
        case 0x63:      // MOV H, E
            state->h = state->e;
            break;
        case 0x64:      // MOV H, H
            state->h = state->h;
            break;
        case 0x65:      // MOV, H, L
            state->h = state->l;
            break;
        case 0x66:      // MOV H, M
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->h = state->memory[offset];
            }
            break;
        case 0x67:      // MOV H,A
            state->h = state->a;
            break;
        case 0x68:      // MOV L,B
            state->l = state->b;
            break;
        case 0x69:      // MOV L, C
            state->l = state->c;
            break;
        case 0x6A:      // MOV L, D
            state->l = state->d;
            break;
        case 0x6B:      // MOV L,E
            state->l = state->e;
            break;
        case 0x6C:      // MOV L, H
            state->l = state->h;
            break;
        case 0x6D:      // MOV L,L
            state->l = state->l;
            break;
        case 0x6E:      // MOV L, M
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->l = state->memory[offset];
            }
            break;
        case 0x6F:      // MOV L,A
            state->l = state->a;
            break;
        case 0x70:      // MOV M,B
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->memory[offset] = state->b;
            }
            break;
        case 0x71:      // MOV M, C
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->memory[offset] = state->c;
            }
            break;
        case 0x72:      // MOV M, D
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->memory[offset] = state->d;
            }
            break;
        case 0x73:      // MOV M, E
            {
                uint16_t offset;
                offset = (state->h << 8) | state->l;
                state->memory[offset] = state->e;
            }
            break;
        case 0x74:      // MOV M, H
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->memory[offset] = state->h;
            }
            break;
        case 0x75:      // MOV M, L
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->memory[offset] = state->l;
            }
            break;
        case 0x77:      // MOV M, A
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->memory[offset] = state->a;
            }
            break;
        case 0x78:      // MOV A,B
            state->a = state->b;
            break;
        case 0x79:      // MOV A, C
            state->a = state->c;
            break;
        case 0x7A:      // MOV A, D
            state->a = state->d;
            break;
        case 0x7B:      // MOV A, E
            state->a = state->e;
            break;
        case 0x7C:      // MOV A,H
            state->a = state->h;
            break;
        case 0x7D:      // MOV A, L
            state->a = state->l;
            break;
        case 0x7E:      // MOV A, M
            {
                uint16_t offset = (state->h << 8) | state->l;
                state->a = state->memory[offset];
            }
            break;
        case 0x7F:      // MOV A, A
            state->a = state->a;
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
        case 0xC4:      // CNZ ADR
            {
                if(state->cc.z == 0)
                {
                    // TODO : I've implemented the call instruciton here, 
                    // but I'm not sure if that is completely correct.
                    uint16_t ret = state->pc + 2;
                    state->memory[state->sp-1] = (ret >> 8) & 0xFF;
                    state->memory[state->sp-2] = ret & 0xFF;
                    state->sp -= 2;
                    state->pc = (opcode[2] << 8) | opcode[1];
                }
                else
                    state->pc += 2;
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
            break;
        case 0xC7:      // RST 0
            state->pc = 0;          // TODO : possibly not correct...
            break;
        case 0xC8:      // RZ  (if Z, RET)
            {
                if(state->cc.z == 1)
                {
                    state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                    state->sp += 2;
                }
            }
            break;
        case 0xC9:      // RET
            {
                state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);
                state->sp += 2;
            }
            break;
        case 0xCA:      // JZ adr
            {
                if(state->cc.z == 1)
                {
                    uint16_t ret = state->pc + 2;
                    state->memory[state->sp-1] = (ret >> 8) & 0xFF;
                    state->memory[state->sp-2] = ret & 0xFF;
                    state->sp -= 2;
                    state->pc = (opcode[2] << 8) | opcode[1];
                }
            }
            break;
        case 0xCC:      // CZ ADR
            {
                if(state->cc.z)
                {
                    uint16_t ret = state->pc + 2;

                    state->memory[state->sp-1] = (ret >> 8) & 0xFF;
                    state->memory[state->sp-2] = ret & 0xFF;
                    state->sp -= 2;
                    state->pc = (opcode[2] << 8) | opcode[1];
                }
            }
            break;

        case 0xCD:      // CALL ADR
            {
                // save reutrn address
                uint16_t ret = state->pc + 2;
                state->memory[state->sp-1] = (ret >> 8) & 0xFF;
                state->memory[state->sp-2] = ret & 0xFF;
                state->sp -= 2;
                state->pc = (opcode[2] << 8) | opcode[1];
            }
            break;
        case 0xCE:      // ACI, d8
            {
                uint16_t res;
                res = state->a + opcode[1];
                arith_set_flags(state, res);
                state->a = res + state->cc.cy;
            }
            break;





        case 0xD1:      // POP D
            {
                state->e = state->memory[state->sp];
                state->d = state->memory[state->sp-1];
                state->sp += 2;
            }
            break;
        case 0xD2:      // JNC adr
            {
                if(!state->cc.cy)
                    state->pc = (opcode[2] << 8) | opcode[1];
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

        case 0xF5:      // PUSH PSW
            {
                uint8_t psw;
                state->memory[state->sp-1] = state->a;
                psw = (state->cc.z       |
                       state->cc.s  << 1 | 
                       state->cc.p  << 2 | 
                       state->cc.cy << 3 | 
                       state->cc.ac << 4);
                state->memory[state->sp-2] = psw;
                state->sp = state->sp - 2;
            }
            break;
        case 0xFE:      // CPI D8
            {
                uint16_t ans = (uint16_t) state->a + (uint16_t) opcode[1];
            }
            break;

        default:
            UnimplementedInstruction(state, opcode[0]);
            return -1;




    }
    state->pc += 1;     

    return 0;       // TODO : what is the correct thing to return here?
}
