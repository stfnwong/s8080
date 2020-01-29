; A simple ROM to test Lexer, Assembler, etc
; This program doesn't strictly do anything useful
;
; Stefan Wong 2020

; This is just taken from lines 259 onwards of the CPU test
MOVI:	MVI	A,077H
		INR	A
		MOV	B,A
		INR	B
		MOV	C,B
		DCR	C
