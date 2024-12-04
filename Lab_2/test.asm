	; Sample program to move data into registers using MOV and LDR
	
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
	MOV R0, #10
	MOV R1, #-1
	MOV R2, #0x10			; 0x is the hexadecimal notation
	MOV R3, #2_1010			; 2_ is the binary notation to represent numbers
	LDR R4, =0x12345678
	
STOP
	B STOP

END