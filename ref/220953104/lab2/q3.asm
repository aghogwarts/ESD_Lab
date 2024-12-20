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
	LDR R7, =DST
	LDR R0, =N; pointer for 1st 128 bit number
	ADD R1, R0, #16; pointer for 2nd 128 bit number
	MOV R2, #4
BACK LDR R3, [R0]
	 LDR R4, [R1]
	 ADCS R5, R3, R4;ADDS R5, R6  ; previous carry
	 ;ADC R6, #0
	 ;ADDS R5, R3, R4
	 STR R5, [R7], #4
	 ;ADC R6, #0   ; carry
	 SUBS R2, #1
	 ADD R0, #4
	 ADD R1, #4
	 BNE BACK
	 STR R6, [R7]
STOP B STOP
N DCD 0xAC3123BC,0xE2DDAEFF,0xE3244143, 0x21214EDE, 0xBC3123BC, 0xE2DDAEFF,0xE3244143, 0x21214EDE
	AREA mydata, DATA, READWRITE
	
	;ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
DST DCD 0, 0, 0, 0, 0  ;RESERVING THE SPACE FOR 5 0'S(FOR 5 VALUE STORING)
	END