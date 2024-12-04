	; add 10 32-bit numbers
	
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
	LDR R5, =DST
	LDR R0, =N
	MOV R1, #10		; counter
	
BACK
	LDR R4, [R0], #4
	ADDS R2, R4		; R2 = R2 + R4
	ADC R3, #0		; Carry
	SUBS R1, #1		; alternative approach skip below two steps
	; SUB R1, #1
	; TEQ R1, #0		; test eqv using XOR
	BNE BACK
	STR R2, [R5], #4
	STR R3, [R5]
	
STOP
	B STOP
	
N 	DCD 0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0x77777777, 0x88888888, 0x99999999, 0xAAAAAAAA
	AREA mydata, DATA, READWRITE
	;ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
	
DST DCD 0, 0  	;RESERVING THE SPACE FOR 2 0'S(FOR 2 VALUE STORING 1 sum and 1 carry)
	END