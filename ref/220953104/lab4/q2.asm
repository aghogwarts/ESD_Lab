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
; 2digit BCD into Hexadecimal
	LDR R0, =N
	LDR R5, =DST
	LDR R1, [R0]
	AND R2, R1, #0x0F
	LSR R1, R1, #4
	AND R4, R1, #0x0F
	MOV R6, #0x0A
	MLA R3, R4, R6, R2
	STR R3, [R5]
STOP B STOP
N DCD 0x87
	AREA mydata, DATA, READWRITE
DST DCD 0
	END