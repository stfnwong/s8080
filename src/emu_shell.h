/* EMULATOR SHELL
 * As per http://www.emulator101.com/emulator-shell.html
 *
 */

#ifndef __EMU_SHELL_H
#define __EMU_SHELL_H

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
typedef struct State8080
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
} State8080;


#endif /*__EMU_SHELL_H*/
