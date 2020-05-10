; TEST_BYTE_LIST
; Unit test for DB/DW instructions
;
; Stefan Wong 2020

TEST_ARGS: DB, "SOME CHARACTER STRING", 0dh, 0ah, 03h
           DB 65h

; Also throw in an incorrect DB and assert that fails 
           ;DB           ; missing arguments
