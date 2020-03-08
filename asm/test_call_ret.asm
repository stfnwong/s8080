; TEST_CALL_RET
; Tests for subroutine call and return instructions
;
; Stefan Wong 2020

; Call instructions
CNZ 0BEEFh
CZ 0BEEFh
CALL 0BEEFh
CC 0BEEFh
CPO 0BEEFh
CPE 0BEEFh
CP 0BEEFh
CM 0BEEFh

; Return instructions
RNZ
RZ
RET
RNC
RPO
RPE
RP
RM
