; TEST_ARTIH
; Tests for the various arithmetic instructions 
;
; Stefan Wong 2020

ARITH_INSTR: ADD C
             SUB A
             ADI 7
             ORA B
             XRA D
             ANA H
             ADC E
             CMP A
             SBB L
             DAD H
             INR D
             INX D
             INR H
             INX H
             DAD B
             LDAX B
