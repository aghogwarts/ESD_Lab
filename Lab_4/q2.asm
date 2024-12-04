	; 2-digit BCD to Hexadecimal converter
	
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
	LDR R5, =HEX
	LDR R1, [R0]
	
	AND R2, R1, #0x0F		; extract LSB using bitwise &
	LSR R1, R1, #4			; feels like an irrelevant step in hindsight just here to beautify appearance
	AND R4, R1, #0x0F
	MOV R6, #0x0A			; storing 10 in R6 to be multiplied at tens place
	MLA R3, R4, R6, R2		; R3=(R4×R6)+R2
	STR R3, [R5]			; final value stored in hex that was calculated as a decimal in the step above
	
STOP
	B STOP
	
N 	DCD 0x87
	AREA mydata, DATA, READWRITE
	
HEX DCD 0
	END