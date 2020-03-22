/*
 * DISASSEM
 * Disassembly routines
 *
 * Stefan Wong 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "disassem.h"
#include "instr.h"
#include "source.h"


/*
 * disassembler_create()
 */
Disassembler* disassembler_create(void)
{
    Disassembler* dis;

    dis = malloc(sizeof(*dis));
    if(!dis)
        return NULL;

    dis->pc = 0;
    dis->codebuffer = NULL;

    return dis;
}

/*
 * disassembler_destroy()
 */
void disassembler_destroy(Disassembler* dis)
{
    free(dis);
}



/* Codebuffer is a valid pointer to 8080 assembly code.
 * PC is the current offset into the code
 *
 * returns the number of bytes of the op
 */

int disasemble_op(uint8_t* codebuffer, int pc)
{

}

/*
disassemble_8080_op_to_console()
*/
int disassemble_8080_op_to_console(uint8_t *codebuffer, int pc)
{
    uint8_t *code = &codebuffer[pc];
    int opbytes = 1;

    fprintf(stdout, "%04X ", pc);
    switch(*code)
    {
        case 0x00:
            fprintf(stdout, "NOP");
            break;
        case 0x01:
            fprintf(stdout, "LXI   B,#$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x02:
            fprintf(stdout, "STAX  B");
            break;
        case 0x03:
            fprintf(stdout, "INX   B");
            break;
        case 0x04:
            fprintf(stdout, "INR   B");
            break;
        case 0x05:
            fprintf(stdout, "DCR   B");
            break;
        case 0x06:
            fprintf(stdout, "MVI   B,#0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0x07:
            fprintf(stdout, "RLC");
            break;
        case 0x08:
            fprintf(stdout, "NOP");
            break;
        case 0x09:
            fprintf(stdout, "DAD   B");
            break;
        case 0x0A:
            fprintf(stdout, "LDAX  B");
            break;
        case 0x0B:
            fprintf(stdout, "DCX   B");
            break;
        case 0x0C:
            fprintf(stdout, "INR   C");
            break;
        case 0x0D:
            fprintf(stdout, "DCR   C");
            break;
        case 0x0E:
            fprintf(stdout, "MVI   C,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x0F:
            fprintf(stdout, "RRC");
            break;
        case 0x10:
            fprintf(stdout, "*NOP");
            break;
        case 0x11:
            fprintf(stdout, "LXI   D,#$%02X%02X", code[2], code[1]);
            break;
        case 0x12:
            fprintf(stdout, "STAX  D");
            break;
        case 0x13:
            fprintf(stdout, "INX   D");
            break;
        case 0x14:
            fprintf(stdout, "INR   D");
            break;
        case 0x15:
            fprintf(stdout, "DCR   D");
            break;
        case 0x16:
            fprintf(stdout, "MVI   D,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x17:
            fprintf(stdout, "DAA");
            break;
        case 0x18:
            fprintf(stdout, "*NOP");
            break;
        case 0x19:
            fprintf(stdout, "DAD   H");
            break;
        case 0x1A:
            fprintf(stdout, "LDAX  D");
            opbytes = 3;
            break;
        case 0x1B:
            fprintf(stdout, "DCX   H");
            break;
        case 0x1C:
            fprintf(stdout, "INR,  E");
            break;
        case 0x1D:
            fprintf(stdout, "DCR   E");
            break;
        case 0x1E:
            fprintf(stdout, "MVI   E,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x1F:
            fprintf(stdout, "RAR");
            break;
        case 0x20:
            fprintf(stdout, "*NOP");
            break;
        case 0x21:
            fprintf(stdout, "LXI   H,#$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x22:
            fprintf(stdout, "SHLD    #$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x23:
            fprintf(stdout, "INX   H");
            break;
        case 0x24:
            fprintf(stdout, "INR   H");
            break;
        case 0x25:
            fprintf(stdout, "DCR   H");
            break;
        case 0x26:
            fprintf(stdout, "MVI   H,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x27:
            fprintf(stdout, "DAA");
            break;
        case 0x28:
            fprintf(stdout, "*NOP");
            break;
        case 0x29:
            fprintf(stdout, "DAD   H");
            break;
        case 0x2A:
            fprintf(stdout, "LHLD  #$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x2B:
            fprintf(stdout, "DCX   H");
            break;
        case 0x2C:
            fprintf(stdout, "INR   L");
            break;
        case 0x2D:
            fprintf(stdout, "DCR   L");
            break;
        case 0x2E:
            fprintf(stdout, "MVI   L,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x2F:
            fprintf(stdout, "CMA");
            break;
        case 0x30:
            fprintf(stdout, "*NOP");
            break;
        case 0x31:
            fprintf(stdout, "LXI  SP,#$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x32:
            fprintf(stdout, "SHLD,   #$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x33:
            fprintf(stdout, "INX   H");
            break;
        case 0x34:
            fprintf(stdout, "INR   H");
            break;
        case 0x35:
            fprintf(stdout, "DCR   H");
            break;
        case 0x36:
            fprintf(stdout, "MVI   H,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x37:
            fprintf(stdout, "STC");
            break;
        case 0x38:
            fprintf(stdout, "*NOP");
            break;
        case 0x39:
            fprintf(stdout, "DAD   SP");
            break;
        case 0x3A:
            fprintf(stdout, "LDA     #$%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0x3B:
            fprintf(stdout, "DCX   SP");
            break;
        case 0x3C:
            fprintf(stdout, "INR   A");
            break;
        case 0x3D:
            fprintf(stdout, "DCR   A");
            break;
        case 0x3E:
            fprintf(stdout, "MVI   A,#$%02X", code[1]);
            opbytes = 2;
            break;
        case 0x3F:
            fprintf(stdout, "CMC");
            break;
        /* ======== 0x40 ======== */
        case 0x40:
            fprintf(stdout, "MOV   B,B");
            break;
        case 0x41:
            fprintf(stdout, "MOV   B,C");
            break;
        case 0x42:
            fprintf(stdout, "MOV   B,D");
            break;
        case 0x43:
            fprintf(stdout, "MOV   B,E");
            break;
        case 0x44:
            fprintf(stdout, "MOV   B,H");
            break;
        case 0x45:
            fprintf(stdout, "MOV   B,L");
            break;
        case 0x46:
            fprintf(stdout, "MOV   B,M");
            break;
        case 0x47:
            fprintf(stdout, "MOV   B,A");
            break;
        case 0x48:
            fprintf(stdout, "MOV   C,B");
            break;
        case 0x49:
            fprintf(stdout, "MOV   C,C");
            break;
        case 0x4A:
            fprintf(stdout, "MOV   C,D");
            break;
        case 0x4B:
            fprintf(stdout, "MOV   C,E");
            break;
        case 0x4C:
            fprintf(stdout, "MOV   C,H");
            break;
        case 0x4D:
            fprintf(stdout, "MOV   C,L");
            break;
        case 0x4E:
            fprintf(stdout, "MOV   C,M");
            break;
        case 0x4F:
            fprintf(stdout, "MOV   C,A");
            break;

        /* ======== 0x50 ======== */
        case 0x50:
            fprintf(stdout, "MOV   D,B");
            break;
        case 0x51:
            fprintf(stdout, "MOV   D,C");
            break;
        case 0x52:
            fprintf(stdout, "MOV   D,D");
            break;
        case 0x53:
            fprintf(stdout, "MOV   D,E");
            break;
        case 0x54:
            fprintf(stdout, "MOV   D,H");
            break;
        case 0x55:
            fprintf(stdout, "MOV   D,L");
            break;
        case 0x56:
            fprintf(stdout, "MOV   D,M");
            break;
        case 0x57:
            fprintf(stdout, "MOV   D,A");
            break;
        case 0x58:
            fprintf(stdout, "MOV   E,B");
            break;
        case 0x59:
            fprintf(stdout, "MOV   E,C");
            break;
        case 0x5A:
            fprintf(stdout, "MOV   E,D");
            break;
        case 0x5B:
            fprintf(stdout, "MOV   E,E");
            break;
        case 0x5C:
            fprintf(stdout, "MOV   E,H");
            break;
        case 0x5D:
            fprintf(stdout, "MOV   E,L");
            break;
        case 0x5E:
            fprintf(stdout, "MOV   E,M");
            break;
        case 0x5F:
            fprintf(stdout, "MOV,  E,A");
            break;
        //case 0x5F:
        //    fprintf(stdout, "MOV   C, A");
        //    break;


        case 0x60:
            fprintf(stdout, "MOV   H,B");
            break;
        case 0x61:
            fprintf(stdout, "MOV   H,C");
            break;
        case 0x62:
            fprintf(stdout, "MOV   H,D");
            break;
        case 0x63:
            fprintf(stdout, "MOV   H,E");
            break;
        case 0x64:
            fprintf(stdout, "MOV   H,H");
            break;
        case 0x65:
            fprintf(stdout, "MOV   H,L");
            break;
        case 0x66:
            fprintf(stdout, "MOV   H,M");
            break;
        case 0x67:
            fprintf(stdout, "MOV   H,A");
            break;
        case 0x68:
            fprintf(stdout, "MOV   L,B");
            break;
        case 0x69:
            fprintf(stdout, "MOV   L,C");
            break;
        case 0x6A:
            fprintf(stdout, "MOV   L,D");
            break;
        case 0x6B:
            fprintf(stdout, "MOV   L,E");
            break;
        case 0x6C:
            fprintf(stdout, "MOV   L,H");
            break;
        case 0x6D:
            fprintf(stdout, "MOV   L,L");
            break;
        case 0x6E:
            fprintf(stdout, "MOV   L,M");
            break;
        case 0x6F:
            fprintf(stdout, "MOV   L,A");
            break;
        case 0x70:
            fprintf(stdout, "MOV   M,B");
            break;
        case 0x71:
            fprintf(stdout, "MOV   M,C");
            break;
        case 0x72:
            fprintf(stdout, "MOV   M,D");
            break;
        case 0x73:
            fprintf(stdout, "MOV   M,E");
            break;
        case 0x74:
            fprintf(stdout, "MOV   M,H");
            break;
        case 0x75:
            fprintf(stdout, "MOV   M,L");
            break;
        case 0x76:
            fprintf(stdout, "HLT");
            break;
        case 0x77:
            fprintf(stdout, "MOV   M,A");
            break;
        case 0x78:
            fprintf(stdout, "MOV   A,B");
            break;
        case 0x79:
            fprintf(stdout, "MOV   A,C");
            break;
        case 0x7A:
            fprintf(stdout, "MOV   A,D");
            break;
        case 0x7B:
            fprintf(stdout, "MOV   A,E");
            break;
        case 0x7C:
            fprintf(stdout, "MOV   A,H");
            break;
        case 0x7D:
            fprintf(stdout, "MOV   A,L");
            break;
        case 0x7E:
            fprintf(stdout, "MOV   A,M");
            break;
        case 0x7F:
            fprintf(stdout, "MOV   A,A");
            break;
        case 0x80:
            fprintf(stdout, "ADD   B");
            break;
        case 0x81:
            fprintf(stdout, "ADD   C");
            break;
        case 0x82:
            fprintf(stdout, "ADD   D");
            break;
        case 0x83:
            fprintf(stdout, "ADD   E");
            break;
        case 0x84:
            fprintf(stdout, "ADD   H");
            break;
        case 0x85:
            fprintf(stdout, "ADD   L");
            break;
        case 0x86:
            fprintf(stdout, "ADD   M");
            break;
        case 0x87:
            fprintf(stdout, "ADD   A");
            break;
        case 0x88:
            fprintf(stdout, "ADC   B");
            break;
        case 0x89:
            fprintf(stdout, "ADC   C");
            break;
        case 0x8A:
            fprintf(stdout, "ADC   D");
            break;
        case 0x8B:
            fprintf(stdout, "ADC   E");
            break;
        case 0x8C:
            fprintf(stdout, "ADC   H");
            break;
        case 0x8D:
            fprintf(stdout, "ADC   L");
            break;
        case 0x8E:
            fprintf(stdout, "ADC   M");
            break;
        case 0x8F:
            fprintf(stdout, "ADC   A");
            break;
        case 0x90:
            fprintf(stdout, "SUB   B");
            break;
        case 0x91:
            fprintf(stdout, "SUB   C");
            break;
        case 0x92:
            fprintf(stdout, "SUB   D");
            break;
        case 0x93:
            fprintf(stdout, "SUB   E");
            break;
        case 0x94:
            fprintf(stdout, "SUB   H");
            break;
        case 0x95:
            fprintf(stdout, "SUB   L");
            break;
        case 0x96:
            fprintf(stdout, "SUB   M");
            break;
        case 0x97:
            fprintf(stdout, "SUB   A");
            break;
        case 0x98:
            fprintf(stdout, "SBB   B");
            break;
        case 0x99:
            fprintf(stdout, "SBB   C");
            break;
        case 0x9A:
            fprintf(stdout, "SBB   D");
            break;
        case 0x9B:
            fprintf(stdout, "SBB   E");
            break;
        case 0x9C:
            fprintf(stdout, "SBB   H");
            break;
        case 0x9D:
            fprintf(stdout, "SBB   L");
            break;
        case 0x9E:
            fprintf(stdout, "SBB   M");
            break;
        case 0x9F:
            fprintf(stdout, "SBB   A");
            break;
        case 0xA0:
            fprintf(stdout, "ANA   B");
            break;
        case 0xA1:
            fprintf(stdout, "ANA   C");
            break;
        case 0xA2:
            fprintf(stdout, "ANA   D");
            break;
        case 0xA3:
            fprintf(stdout, "ANA   E");
            break;
        case 0xA4:
            fprintf(stdout, "ANA   H");
            break;
        case 0xA5:
            fprintf(stdout, "ANA   L");
            break;
        case 0xA6:
            fprintf(stdout, "ANA   M");
            break;
        case 0xA7:
            fprintf(stdout, "ANA   A");
            break;
        case 0xA8:
            fprintf(stdout, "XRA   B");
            break;
        case 0xA9:
            fprintf(stdout, "XRA   C");
            break;
        case 0xAA:
            fprintf(stdout, "XRA   D");
            break;
        case 0xAB:
            fprintf(stdout, "XRA   E");
            break;
        case 0xAC:
            fprintf(stdout, "XRA   H");
            break;
        case 0xAD:
            fprintf(stdout, "XRA   L");
            break;
        case 0xAE:
            fprintf(stdout, "XRA   M");
            break;
        case 0xAF:
            fprintf(stdout, "XRA   A");
            break;
        case 0xB0:
            fprintf(stdout, "ORA   B");
            break;
        case 0xB1:
            fprintf(stdout, "ORA   C");
            break;
        case 0xB2:
            fprintf(stdout, "ORA   D");
            break;
        case 0xB3:
            fprintf(stdout, "ORA   E");
            break;
        case 0xB4:
            fprintf(stdout, "ORA   H");
            break;
        case 0xB5:
            fprintf(stdout, "ORA   L");
            break;
        case 0xB6:
            fprintf(stdout, "ORA   M");
            break;
        case 0xB7:
            fprintf(stdout, "ORA   A");
            break;
        case 0xB8:
            fprintf(stdout, "CMP   B");
            break;
        case 0xB9:
            fprintf(stdout, "CMP   C");
            break;
        case 0xBA:
            fprintf(stdout, "CMP   D");
            break;
        case 0xBB:
            fprintf(stdout, "CMP   E");
            break;
        case 0xBC:
            fprintf(stdout, "CMP   H");
            break;
        case 0xBD:
            fprintf(stdout, "CMP   L");
            break;
        case 0xBE:
            fprintf(stdout, "CMP   M");
            break;
        case 0xBF:
            fprintf(stdout, "CMP   A");
            break;
        /* ======== 0xc0 ======== */
        case 0xC0:
            fprintf(stdout, "RNZ    ");
            break;
        case 0xC1:
            fprintf(stdout, "POP   B");
            break;
        case 0xC2:
            fprintf(stdout, "JNZ   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xC3:
            fprintf(stdout, "JMP   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xC4:
            fprintf(stdout, "CNZ   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xC5:
            fprintf(stdout, "PUSH  B");
            break;
        case 0xC6:
            fprintf(stdout, "ADI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xC7:
            fprintf(stdout, "RST   0");
            break;
        case 0xC8:
            fprintf(stdout, "RZ    ");
            break;
        case 0xC9:
            fprintf(stdout, "RET   ");
            break;
        case 0xCA:
            fprintf(stdout, "JZ    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xCB:
            fprintf(stdout, "*JMP  #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xCC:
            fprintf(stdout, "CZ    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xCD:
            fprintf(stdout, "CALL  #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xCE:
            fprintf(stdout, "ACI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xCF:
            fprintf(stdout, "RST   1");
            break;


        /* ======== 0xD0 ======== */
        case 0xD0:
            fprintf(stdout, "RNZ   ");
            break;
        case 0xD1:
            fprintf(stdout, "POP   D");
            break;
        case 0xD2:
            fprintf(stdout, "JNC   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xD3:
            fprintf(stdout, "OUT   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xD4:
            fprintf(stdout, "CNC   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xD5:
            fprintf(stdout, "PUSH  D");
            break;
        case 0xD6:
            fprintf(stdout, "SUI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xD7:
            fprintf(stdout, "RST   2");
            break;
        case 0xD8:
            fprintf(stdout, "RC    ");
            break;
        case 0xD9:
            fprintf(stdout, "*RET  ");
            break;
        case 0xDA:
            fprintf(stdout, "JC    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xDB:
            fprintf(stdout, "IN    #0x%02x", code[1]);
            opbytes = 2;
            break;
        case 0xDC:
            fprintf(stdout, "CC    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xDD:
            fprintf(stdout, "*CALL #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xDE:
            fprintf(stdout, "XRI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xDF:
            fprintf(stdout, "RST   5");
            break;

        /* ======== 0xE0 ======== */
        case 0xE0:
            fprintf(stdout, "RPO   ");
            break;
        case 0xE1:
            fprintf(stdout, "POP   H");
            break;
        case 0xE2:
            fprintf(stdout, "JNC   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xE3:
            fprintf(stdout, "XTHL  ");
            break;
        case 0xE4:
            fprintf(stdout, "CPO   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xE5:
            fprintf(stdout, "PUSH  H");
            break;
        case 0xE6:
            fprintf(stdout, "ANI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xE7:
            fprintf(stdout, "RST   4");
            break;
        case 0xE8:
            fprintf(stdout, "RPE   ");
            break;
        case 0xE9:
            fprintf(stdout, "PCHL  ");
            break;
        case 0xEA:
            fprintf(stdout, "JPE   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xEB:
            fprintf(stdout, "XCHG  ");
            break;
        case 0xEC:
            fprintf(stdout, "CPE   #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xED:
            fprintf(stdout, "*CALL #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xEE:
            fprintf(stdout, "XRI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xEF:
            fprintf(stdout, "RST   5");
            break;

        /* ======== 0xF0 ======== */
        case 0xF0:
            fprintf(stdout, "RP    ");
            break;
        case 0xF1:
            fprintf(stdout, "POP   PSW");
            break;
        case 0xF2:
            fprintf(stdout, "JP    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xF3:
            fprintf(stdout, "DI    ");
            break;
        case 0xF4:
            fprintf(stdout, "CP    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xF5:
            fprintf(stdout, "PUSH  PSW");
            break;
        case 0xF6:
            fprintf(stdout, "ORI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xF7:
            fprintf(stdout, "RST   6");
            break;
        case 0xF8:
            fprintf(stdout, "RM    ");
            break;
        case 0xF9:
            fprintf(stdout, "SPHL  ");
            break;
        case 0xFA:
            fprintf(stdout, "JM    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xFB:
            fprintf(stdout, "EI    ");
            break;
        case 0xFC:
            fprintf(stdout, "CM    #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xFD:
            fprintf(stdout, "*CALL #0x%02X%02X", code[2], code[1]);
            opbytes = 3;
            break;
        case 0xFE:
            fprintf(stdout, "CPI   #0x%02X", code[1]);
            opbytes = 2;
            break;
        case 0xFF:
            fprintf(stdout, "RST   7");
            break;
    }

    fprintf(stdout, "\n");

    return opbytes;
}

 





