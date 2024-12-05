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
	
    LDR R0, =ARR           
    LDR R1, =DST            
    MOV R2, #6              
    MOV R3, #0              

    
    MOV R4, #0              
CopyLoop
    CMP R4, R2              
    BGE DoneCopying         
    ADD R5, R0, R4, LSL #2  
    LDR R6, [R5]            
    ADD R7, R1, R4, LSL #2  
    STR R6, [R7]            
    ADD R4, R4, #1          
    B CopyLoop             

DoneCopying
    

OuterLoop
    CMP R3, R2              
    BGE Done                

    MOV R4, R3              
    MOV R5, R3              

InnerLoop
    ADD R6, R1, R5, LSL #2  
    LDR R7, [R6]            
    ADD R8, R1, R4, LSL #2  
    LDR R9, [R8]            

    CMP R7, R9              
    BGE SkipSwap           

    MOV R4, R5              

SkipSwap
    ADD R5, R5, #1         
    CMP R5, R2              
    BLT InnerLoop           


    ADD R6, R1, R3, LSL #2  
    LDR R7, [R6]            
    ADD R8, R1, R4, LSL #2  
    LDR R9, [R8]            

    STR R9, [R6]           
    STR R7, [R8]           

    ADD R3, R3, #1         
    B OuterLoop            

Done
    B Done            

ARR DCD 0x1, 0x5, 0x2, 0x6, 0x3, 0x4  

	AREA Mydata, DATA, READWRITE
DST  DCD 0, 0, 0, 0, 0, 0      
	END


