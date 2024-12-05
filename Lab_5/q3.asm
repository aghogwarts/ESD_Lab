	; search an ele from an array of 10 32-bit numbers using linear search
	
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
	LDR R0, =N			; load base addr
	LDR R1, =NUM
	LDR R1, [R1]		; load search ele from address
	LDR R4, =Result		; load res addr
	MOV R2, #10			; loop counter
	MOV R6, #10			; index counter
	
UP  
	LDR R3, [R0], #4	; load cur ele into R3 and move to next
	CMP R1, R3			; compare to check
	BEQ EXIT
	SUB R2, #1			; count--
	CMP R2, #0			; recursion cmp
	BNE UP
	
EXIT 
	CMP R2, #0			; check if loop has ended due to no matches
	ADDNE R5, #1		; if != flag is True (1)
	SUB R6, R6, R2		; index calc
	STR R5, [R4]
	STR R6, [R4, #4]	
	
STOP
	B STOP

N 	DCD 0x1C2E34D7, 0xA13B4EF8, 0xD32E54A6, 0x543A5EBC, 0xABED2A4E, 0x24141245, 0xE196DA4C, 0x9324EAC2, 0x654ECD2A, 0xABCDDEAF

NUM DCD 0x543A5EBC	
	AREA mydata, DATA, READWRITE
	
Result DCW 0
	END
