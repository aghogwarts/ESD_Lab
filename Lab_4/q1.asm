	; 2-digit hexadecimal number to ASCII
	
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
	LDR R0, =N
	LDR R1, =ASCII
	LDR R2, [R0]
	
	AND R3, R2, #0x0F		; extract LSB using bitwise &
	CMP R3, #0x9			; compare with 9 to see if numeric or alphabetic
	ADDHI R3, #0x37			; if R3>9 i.e. alphabet add 0x37 or 55 to convert A-Z
	ADDLS R3, #0x30			; if R3<9 i.e. number add 0x30 or 48 to convert 0-9
	STR R3, [R1], #1		; store at R1 then increment R1
	
	LSR R3, R2, #4			; logical shift right by 4 to eliminate the LSB and operate on MSB
	CMP R3, #0x9			; repeat same logic as above ^_^
	ADDHI R3, #0x37
	ADDLS R3, #0x30
	STR R3, [R1]			; final value at R1 as ASCII values of both digits
	
STOP
	B STOP
	
N 	DCD 0x5D
	AREA mydata, DATA, READWRITE
	
ASCII DCD 0
	END