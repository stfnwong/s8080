/* 
 *
 * CPU 
 * 8080 CPU stuff.
 */

#ifndef __CPU_H
#define __CPU_H

//#define CPU_DIAG
#define CPU_MEM_SIZE 0x10000

#include <stdint.h>

// Condition code
typedef struct 
{
    uint8_t z   :1;         //zero
    uint8_t s   :1;         //sign
    uint8_t p   :1;         //parity
    uint8_t cy  :1;         //carry 
    uint8_t ac  :1;
    uint8_t pad :3;
} ConditionCodes;

// State structure 
typedef struct CPUState
{
    uint8_t        a;
    uint8_t        b;
    uint8_t        c;
    uint8_t        d;
    uint8_t        e;
    uint8_t        h;
    uint8_t        l;
    uint16_t       sp;
    uint16_t       pc;
    uint8_t        *memory;
    ConditionCodes cc;
    uint8_t        int_enable;
    uint16_t       shift_reg;
    uint16_t       shift_amount;
    //int            mem_size;
    // function pointers for I/O
    uint8_t (*inport)(void* cpu, uint8_t port);
    void    (*outport)(void* cpu, uint8_t port, uint8_t data);
} CPUState;

// Get a new emulator state
CPUState *cpu_create(void);
void      cpu_destroy(CPUState *state);
void      cpu_print_state(CPUState* state);

// Memory 
void    cpu_print_memory(CPUState* state, int n, int offset);
void    cpu_clear_memory(CPUState* state);
void    cpu_write_mem(CPUState* state, uint16_t addr, uint8_t data);
uint8_t cpu_read_mem(CPUState* state, uint16_t addr);
int     cpu_load_memory(CPUState* state, const char* filename, int offset);


// default I/O ports
static uint8_t cpu_default_inport(void* cpu, uint8_t port);
static void    cpu_default_outport(void* cpu, uint8_t port, uint8_t data);


// Operation
void cpu_shift_register(CPUState* state);
int  cpu_run(CPUState* state, long cycles, int verbose);
int  cpu_exec(CPUState *state);
void UnimplementedInstruction(CPUState *state, unsigned char opcode);

// ======== INLINE METHODS ======== //
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
static inline void arith_set_flags(CPUState *state, uint16_t ans)
{
    state->cc.z  = ((ans & 0xFF) == 0);  // zero flag. 
    state->cc.s  = ((ans & 0x80) != 0);  // sign flag. Set if bit 7 is set 
    state->cc.cy = ((ans > 0xff) != 0);  // carry flag
    state->cc.p = Parity2(ans & 0xFF, 16);    // parity flag
} 

static inline void logic_set_flags(CPUState *state)
{
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.s = (0x80 == (state->a & 0x80));
    state->cc.p = Parity(state->a);
}



#endif /*__CPU_H*/
