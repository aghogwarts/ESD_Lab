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
	MOV R3, #5
	LDR R2, =DST
	LDR R0, =N
BACK LDR R1, [R0], #4
	STR R1, [R2], #4
	;LOOP CODE
	SUB R3, #1
	;WHEN R3 = 0, THEN 0 FLAG(XPSR) = 0
	CMP R3, #0
	;B MEANS BRANCH
	;BNE MEANS IT CHECKS WHETHER 0 FLAG IN XPSR IS 1 OR NOT
	;SEE uVISION HELP CLICK ON ARM INSTRUCTION CLICK B YOU WILL SEE NE ALSO
	BNE BACK
STOP B STOP
N DCD 0x12345678, 0xA1340000, 0x325465A3, 0x54351ABE, 0xABED1220
	AREA mydata, DATA, READWRITE
	
	;ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
DST DCD 0, 0, 0, 0, 0  ;RESERVING THE SPACE FOR 5 0'S(FOR 5 VALUE STORING)
	END