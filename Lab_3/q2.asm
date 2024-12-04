	; add 2 128-bit numbers
	
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
	;LDR R0, =N				; pointer for 1st 128 bit number
	;ADD R1, R0, #16		; pointer for 2nd 128 bit number
	LDR R0, =N1		; alternative pointing
	LDR R1, =N2
	MOV R2, #4
	
BACK
	LDR R3, [R0]          
    LDR R4, [R1]          
	ADDS R3, R6
	MOV R6, #0
    ADCS R5, R3, R4     ; Add with carry (R5 = R3 + R4 + carry)
    STR R5, [R7], #4    
    ADC R6, #0           
    
    ; Update pointers and loop counter
    SUBS R2, #1      	; Decrement the counter
    ADD R0, R0, #4      ; Move to the next 32-bit word in the first number
    ADD R1, R1, #4      ; Move to the next 32-bit word in the second number
    BNE BACK             

    STR R6, [R7]        ; Store final carry value in the result space
	
STOP
	B STOP
	
;N 	DCD 0xAC3123BC, 0xE2DDAEFF, 0xE3244143, 0x21214EDE, 0xBC3123BC, 0xE2DDAEFF, 0xE3244143, 0x21214EDE
N1 	DCD 0xAC3123BC, 0xE2DDAEFF, 0xE3244143, 0x21214EDE
N2 	DCD 0xBC3123BC, 0xE2DDAEFF, 0xE3244143, 0x21214EDE

	AREA mydata, DATA, READWRITE
	
	;ALWAYS RESERVE RQUIRED NUMBER OF MEMORY SPACE AND THEN USE IT
DST DCD 0, 0, 0, 0, 0  ;RESERVING THE SPACE FOR 5 0'S(FOR 5 VALUE STORING)
	END