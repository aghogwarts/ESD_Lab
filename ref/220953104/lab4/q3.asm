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
; hex to bcd
	LDR R0, =N
	LDR R1, [R0]
UP2
	CMP R1, #0
	BEQ EXIT2         ; z = 1, i.e R1 = 0
	MOV R3, #0
UP1
	CMP R1, #0x0A
	BLO EXIT1         ; if R1 < 10 then go to exit 1
	SUB R1, #0x0A
	ADD R3, #1
	B UP1
EXIT1 
	LSL R1, R2        ; shifting R1 by R2 bits
	ADD R2, #4
	ADD R4, R1
	MOV R1, R3
	B UP2
EXIT2
	LDR R0, =BCD
	STR R4, [R0]
	
STOP B STOP
N DCD 0x80
	AREA mydata, DATA, READWRITE
BCD DCD 0
	END