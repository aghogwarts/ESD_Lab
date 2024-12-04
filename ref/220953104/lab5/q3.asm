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
	LDR R1, =NUM
	LDR R1, [R1]
	LDR R4, =result
	MOV R2, #10
UP  
	LDR R3, [R0], #4
	CMP R1, R3
	BEQ EXIT
	SUB R2, #1
	CMP R2, #0
	BNE UP
EXIT 
	CMP R2, #0
	ADDNE R5, #1
	STR R5, [R4]
	
STOP B STOP
N DCD 0x1C2E34D7, 0xA13B4EF8, 0xD32E54A6, 0x543A5EBC, 0xABED2A4E, 0x24141245, 0xE196DA4C, 0x9324EAC2, 0x654ECD2A, 0xABCDDEAF
NUM DCD 0xA13B4EF8	
	AREA mydata, DATA, READWRITE
result DCW 0 
	END