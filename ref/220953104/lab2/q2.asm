	AREA RESET, DATA, READONLY
	EXPORT __Vectors
__Vectors
	DCD 0x10001000
	DCD Reset_Handler
	ALIGN
	AREA mycode, CODE, READONLY
	ENTRY
	EXPORT Reset_Handler
Reset_Handler
	MOV R3, #10
	LDR R0, =N
	LDR R1, =DST
UP1 LDR R2, [R0], #4
	STR R2, [R1], #4
    SUB R3, #1
    CMP R3, #0
	BNE UP1
	MOV R3, #5
	LDR R2, =DST
	ADD R4, R2, #36
BACK LDR R1, [R2]
	 LDR R5, [R4]
	 STR R1, [R4], #-4
	 STR R5, [R2], #4
	 SUB R3, #1
	 CMP R3, #0
	 BNE BACK
STOP B STOP
N DCD 0x1C2E34D7, 0xA13B4EF8, 0xD32E54A6, 0x543A5EBC, 0xABED2A4E, 0x24141245, 0xE196DA4C, 0x9324EAC2, 0x654ECD2A, 0xABCDDEAF
	AREA mydata, DATA, READWRITE
	
	;ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
DST DCD 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  ;RESERVING THE SPACE FOR 5 0'S(FOR 5 VALUE STORING)
	END
;right shift by 1 will mean divide by 2
; you can do that instead of hard coding value of R3
;also instead of using a register R3, you can use SIZE EQU 5 which means size = 5 but no memory space is allotted for size variable 
