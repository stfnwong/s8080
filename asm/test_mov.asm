; TEST_MOV
; Tests for the various MOV instructions 
;
; Stefan Wong 2020

MOVE_INSTR: MOV A, B
            MOV B, C
            MOV A, M
            PUSH D
            MOV E, A
            MVI C, 2
            POP D
