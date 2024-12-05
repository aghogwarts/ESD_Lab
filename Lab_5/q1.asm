	; Selection Sort Implementation

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
    MOV R1, #10       ; Number of elements in the array
    LDR R0, =N        ; Load base address of array N
    LDR R2, =Result   ; Load base address of Result
    MOV R4, #0        ; Initialize index to 0

; Copy array from N to Result
UP  
    LDR R3, [R0, R4]  ; Load element from N
    STR R3, [R2, R4]  ; Store element in Result
    ADD R4, #4        ; Move to next element (word-aligned)
    SUB R1, #1        ; Decrement counter
    CMP R1, #0        ; Check if all elements are copied
    BHI UP            ; If not, repeat loop

    ; Reset variables for sorting
    MOV R4, #0        ; Initialize outer loop index
    LDR R2, =Result   ; Base address of Result
    MOV R1, #10        ; Outer loop counter (9 iterations for 10 elements)

; Outer Loop
OUTER_LOOP
    SUB R1, #1        ; Decrement outer loop counter
    CMP R1, #0        ; Check if sorting is complete
    BEQ EXIT          ; Exit if done
    ADD R5, R2, R4    ; R5 points to current minimum element
    LDR R8, [R5]      ; Load current minimum element
    MOV R6, R4        ; Save index of current minimum
    MOV R7, R4        ; Reset inner loop index

; Inner Loop
INNER_LOOP
    ADD R7, #4        ; Move to next element in array
    CMP R7, #40       ; Check if end of array is reached (10 elements x 4 bytes)
    BEQ SWAP_ELEMENTS ; If end reached, swap minimum with current position
    ADD R3, R2, R7    ; Calculate address of the next element
    LDR R9, [R3]      ; Load the next element
    CMP R9, R8        ; Compare next element with current minimum
    BHS INNER_LOOP    ; If the next element is larger or equal, continue
    MOV R8, R9        ; Update current minimum value
    MOV R6, R7        ; Update index of current minimum
    B INNER_LOOP      ; Continue inner loop

; Swap minimum with current position
SWAP_ELEMENTS
    ADD R3, R2, R4    ; Calculate address of the current position
    LDR R9, [R3]      ; Load the current element
    ADD R3, R2, R6    ; Calculate address of the minimum element
    STR R9, [R3]      ; Store the current element in the minimum's position
    ADD R3, R2, R4    ; Recalculate address of the current position
    STR R8, [R3]      ; Store the minimum value in the current position
    ADD R4, #4        ; Move to the next position in the array
    B OUTER_LOOP      ; Continue outer loop

EXIT
STOP
    B STOP            ; Halt the program

; Data Section
N 	DCD 0xA, 0x5, 0x2, 0x8, 0x3, 0x4, 0xC, 0x7, 0x6, 0x1
	AREA mydata, DATA, READWRITE
	
Result DCD 0 ; Reserve space for sorted array
	END
