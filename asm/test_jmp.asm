; TEST_JMP
; Tests for the various control flow instructions 
;
; Stefan Wong 2020

SOME_LABEL: ANI 0

OTHER_LABEL: ADI 7

JMP_INSTR: JMP SOME_LABEL
           JMP OTHER_LABEL
           JZ  SOME_LABEL
           JNC OTHER_LABEL
           JP  SOME_LABEL
           JC  OTHER_LABEL
           JM  SOME_LABEL
