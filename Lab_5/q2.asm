	; factorial of an unsigned number
	
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
	LDR R1, [R0]
	BL FACT
	LDR R0, =Result
	STR R1, [R0]
	
STOP
	B STOP

FACT
	CMP R1, #1			; compare cur with 1 if equal exit
	BEQ EXIT
	PUSH {R1, LR}		; push cur no and link reg to stack
	SUB R1, #1			; decrement to multiply with n-1
	BL FACT				; loop till n,n-1...1 is stored in stack
	
EXIT
	POP{R2, LR}			; pop prev val of R1 into R2 and restore LR
	MUL R1, R2			; n * n-1
	BX LR				; return to call interrupt
	
N 	DCD 0x5
	AREA mydata, DATA, READWRITE
	
Result DCD 0
	END