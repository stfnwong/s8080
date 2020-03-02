; TEST_INSTR
; All instructions in various combinations for testing Lexer and 
; Assembler components.
;
; Note that this code makes no actual sense
;
; Stefan Wong 2020


MOVE_INSTR: MOV A, B
            MOV B, C
            MOV A, M
            PUSH D
            MOV E, A
            MVI C, 2
            POP D

MEM_INSTR:
            LXI SP, 300H

ARITH_INSTR:
            PUSH A
            PUSH C
            ADD A
            SUB B
            ADI 7

