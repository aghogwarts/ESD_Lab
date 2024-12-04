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
	LDR R6, =DST         
    LDR R0, =N           
    LDR R1, [R0], #4     
    LDR R2, [R0]         
    MOV R3, #1           

LCM_LOOP
    MUL R4, R3, R1       ; R4 = i * a
    MOV R5, R4           

REM_LOOP
    CMP R5, R2           
    BLO CHECK_REMAINDER  ; If R5 < b, go to check remainder
    SUB R5, R5, R2       
    B REM_LOOP           

CHECK_REMAINDER
    CMP R5, #0           
    BEQ LCM_FOUND        ; If remainder is 0, we found the LCM
    ADD R3, R3, #1       
    B LCM_LOOP           

LCM_FOUND
    STR R4, [R6]         

STOP B STOP               

N DCD 0x00000002, 0x00000003 
	AREA mydata, DATA, READWRITE
DST DCD 0                    

    END