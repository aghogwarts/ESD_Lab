	;for shifting an array by S of Size size 
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
	MOV R3, #10
	LDR R0, =N
	LDR R1, =DST
	
UP1 LDR R2, [R0], #4
	STR R2, [R1], #4
    SUB R3, #1
    CMP R3, #0
	BNE UP1
	MOV R3, #10
	LDR R2, =(DST + ((size - 1) + S) * 4) 	;LAST LOCATION WHERE LAST VALUE WILL BE SHIFTED SO IF SIZE IS 5 AND SHIFT IS 3, THEN THIS WILL POINT TO LOCATION 8 SO THAT 5 CAN BE STORED AT 8
	LDR R0, =(DST + (size - 1) * 4) 		;TO ACESS VALUE WRITTEN IN 5 FOR THIS CASE
	
BACK LDR R1, [R0], #-4
	STR R1, [R2], #-4
	; LOOP CODE
	SUB R3, #1
	; WHEN R3 = 0, THEN 0 FLAG(XPSR) = 0
	CMP R3, #0
	; B MEANS BRANCH
	; BNE MEANS IT CHECKS WHETHER 0 FLAG IN XPSR IS 1 OR NOT
	; SEE uVISION HELP CLICK ON ARM INSTRUCTION CLICK B YOU WILL SEE NE ALSO
	BNE BACK
	
STOP B STOP
N DCD 0x1C2E34D7, 0xA13B4EF8, 0xD32E54A6, 0x543A5EBC, 0xABED2A4E, 0x24141245, 0xE196DA4C, 0x9324EAC2, 0x654ECD2A, 0xABCDDEAF

size EQU 10
S EQU 3		; NO_OF SHIFTS
	AREA mydata, DATA, READWRITE
	; ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
	; N DCD 0x12345678, 0xA1340000, 0x325465A3, 0x54351ABE, 0xABED1220, 0, 0, 0  ;RESERVING THE SPACE FOR 3 0'S(FOR 3 VALUE Shifting) this wont get initialized you have to put all 0's; double click on the memory 1 0x1000000 and then initialize for each address space

DST DCD 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	END