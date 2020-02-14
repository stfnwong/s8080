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

typedef enum
{
    CPU_REG_A,
    CPU_REG_B,
    CPU_REG_C,
    CPU_REG_D,
    CPU_REG_E,
    CPU_REG_H,
    CPU_REG_L,
} CPUReg;

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
    // function pointers for I/O
    uint8_t (*inport)(void* cpu, uint8_t port);
    void    (*outport)(void* cpu, uint8_t port, uint8_t data);
} CPUState;

// default I/O ports
static uint8_t cpu_default_inport(void* cpu, uint8_t port);
static void    cpu_default_outport(void* cpu, uint8_t port, uint8_t data);

// Create and destroy state objects 
CPUState *cpu_create(void);
void cpu_destroy(CPUState *state);

// ======== Instruction processing patterns ======== //
void     cpu_move_reg(CPUState* state, CPUReg src, CPUReg dst);
void     cpu_jump(CPUState* state, uint16_t addr);      // inline?
void     cpu_stack_push(CPUState* state, uint16_t val);
uint16_t cpu_stack_pop(CPUState* state);
uint16_t cpu_read_hl(CPUState* state);
void     cpu_write_hl(CPUState* state, uint16_t val);
uint8_t  cpu_read_mem(CPUState* state, uint16_t addr);
void     cpu_write_mem(CPUState* state, uint16_t addr, uint8_t data);

void     cpu_interrupt(CPUState* state, uint8_t n);
void     cpu_print_state(CPUState *state);
int      cpu_load_memory(CPUState *state, const char *filename, int offset);
void     cpu_print_memory(CPUState* state, int n, int offset);
void     cpu_clear_memory(CPUState* state);
// Dumps console output when an unimplemented instruction is encountered
void     cpu_unimplemented_instr(CPUState *state, unsigned char opcode);

// Operation
void     cpu_shift_register(CPUState* state);
int      cpu_run(CPUState* state, long cycles, int verbose);
int      cpu_exec(CPUState *state);


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
static inline void cpu_arith_set_flags(CPUState* state, uint16_t ans)
{
    state->cc.z  = ((ans & 0xFF) == 0);  // zero flag. 
    state->cc.s  = ((ans & 0x80) != 0);  // sign flag. Set if bit 7 is set 
    state->cc.cy = ((ans > 0xff) != 0);  // carry flag
    state->cc.p = Parity2(ans & 0xFF, 16);    // parity flag
} 

static inline void cpu_arith_set_borrow16(CPUState* state, uint16_t ans)
{
    state->cc.s  = (ans & 0x8000) ? 1 : 0;
    state->cc.p  = Parity(state->a);
    state->cc.cy = ((ans & 0xFF00) > 0);
    state->cc.z  = (ans == 0);
}

static inline void cpu_arith_set_borrow32(CPUState* state, uint32_t ans)
{
    state->cc.s  = (ans & 0x80000000) ? 1 : 0;
    state->cc.p  = Parity(state->a);
    state->cc.cy = ((ans & 0xFFFF0000) > 0);
    state->cc.z  = (ans == 0);
}

static inline void cpu_logic_set_flags(CPUState* state)
{
    state->cc.cy = 0;
    state->cc.ac = 0;
    state->cc.s = (0x80 == (state->a & 0x80));
    state->cc.p = Parity(state->a);
}

#endif /*__CPU_H*/
