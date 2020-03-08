; TEST_LONG_INSTR
; Long (3 byte) instructions for 8080 assembler
; 
; Stefan Wong 2020

TEST_START: ADI 0
; Valid LXI intructions 
            LXI B 250h
            LXI D 100h
            LXI H 5
            LXI SP 22h
; LXI is not supposed to accept A, C, or L as args,
; but we could relax that and just assemble A as (AB), 
; C as (CD) and so on...
