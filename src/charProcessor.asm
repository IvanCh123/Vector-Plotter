;User funtion processor. Checks for different conditions.
;1.Mathematical sense of the expression.
;2.Character allowed validation.
;3.Term limit per each section of the mathematical expression.
;4.Variable limit per each term.
;5.Sense of the intervals.
;Also converts the characters values into an internal notation, and fill 
;two vectors: one with the vector function and other with the interval 
;limits, amount of variables, and step size inside the interval.
;Returns the size of the final vector function.

;***********************************************************************
;***********************************************************************
;								DATA SECTION
section .data
variableX  		 db 0 ; Variable x present
variableY  		 db 0 ; Variable y present
operandTermCount db 0 ; Amount of operands inside a term
termCount 		 db 0 ; Amount of terms inside a dimension
dimensionCount	 db 1 ; Dimensions present inside the function
prevSpecialFlag	 db 0 ; If true and currentChar is not a number or variable then go invalidSintax
intervalCount 	 db 0 ; Amount of intervals per variable
commaPresent 	 db 1 ; If 0  go invalidSintax 
cleaningMask	 dq 255; Clean the first 7 bytes of a register
commaCharacter	 dq 44.0; Divides the intervals 
pointCount       dq 0  ; Amount of posible points acording to the interval parameters
intervalXCount	 db 0
intervalYCount 	 db 0
yVariableValidate db 0
;***********************************************************************
;***********************************************************************
; 								BSS SECTION
section .bss
vectorFunction	resw 100
intervals	resq 15
;***********************************************************************
;***********************************************************************
; 								CODE SECTION
;First->	rdi:	charVectorFunction	char array
;Second->	rsi:	vectorFunction		int array
;Third->	rdx:	intervals			double array
section .text
extern floatingPoint

global charProcessor
charProcessor:


push r10 ; Amount of chars evaluated 
push r11 ; Temporal reservoir 								
push r12 ; current char
push r13
push r14
push r15 ; current position in rsi and rdx, both used in different stages.
xor r10, r10
xor r11, r11
xor r12, r12
xor r13, r13
xor r14, r14
xor r15, r15

characterSwitch:
mov r12b, byte[rdi+r10]
and r12, qword[cleaningMask]
cmp r12b, 121; Checks if it is a variable y
je  isVariableY
cmp r12b, 120; Checks if it is a variable x
je  isVariableX
cmp r12b, 99; Check if it is a predefined variable
jge isPredefined
cmp r12b, 48; Check if it is a number
jge isNumber
jmp invalidSintax

;***********************************************************************
;								Variables Section
isVariableX:
mov byte[prevSpecialFlag],0
inc byte[variableX]
inc byte[operandTermCount]
sub r12,130
mov dword[rsi+r15],r12d
add r15,4
cmp byte[operandTermCount],4
jg  invalidSintax
jmp operatorCheck


isVariableY:
mov byte[prevSpecialFlag],0
inc byte[variableY]
inc byte[operandTermCount]
sub r12,130
mov dword[rsi+r15],r12d
add r15,4
cmp byte[operandTermCount],4
jg  invalidSintax
jmp operatorCheck

;***********************************************************************
;								Especial Section
isPredefined:
cmp byte[prevSpecialFlag],1		
je 	invalidSintax
inc byte[operandTermCount]
sub r12,130
mov dword[rsi+r15],r12d
add r15,4
cmp byte[operandTermCount],4
jg  invalidSintax
inc byte[prevSpecialFlag]
inc r10
jmp characterSwitch

;***********************************************************************
;								Number Section
isNumber:
mov byte[prevSpecialFlag],0
inc byte[operandTermCount]
cmp byte[operandTermCount],4
jg  invalidSintax
xor r11,r11
sub r12b,48						; convert char into a digit from 0 up to 9
mov r11,r12						; r11 saves the number in progress
inc r10


; Fuse the digits into a decimal base number 
fuseDigits:
mov r12b, byte[rdi+r10]
cmp r12b, 32					; check for an empty space, this space will be absorbed by operator check
je  insertNumber
mov r13,r11
shl r11, 3						; multiply by 8
shl r13, 1						; multiply by 2
add r11,r13						; r11*8+r11*2 = r11*10
sub r12,48
add r11,r12					    ; 52*8 + 52*2 + 3 = 523 // r11=52 r12=3 
inc r10
jmp fuseDigits



insertNumber:						 
mov dword[rsi+r15], r11d
add r15, 4
xor r11,r11
xor r13,r13
jmp operatorCheck

;***********************************************************************
;							Operators Section
operatorCheck:
inc r10
mov r12b, byte[rdi+r10]
and r12, qword[cleaningMask]    ; leaves just the lowest 8 bits
cmp r12b, 40 					; check if it is a open parenthesis
je	isOPt
cmp r12b, 42 					; check if it is a mul
je	isMul
cmp r12b, 43 					; check if it is a addition
je	isAdd
cmp r12b, 44 					; check if it is a comma
je	isCom
cmp r12b, 45 					; check if it is a substraction
je	isSub
cmp r12b, 47 					; check if it is a division
je	isDiv
jmp invalidSintax


isMul:
cmp byte[prevSpecialFlag],0 	;check if previous char was a reserved one
jg  invalidSintax
mov dword[rsi+r15],-88
add r15, 4
inc r10
jmp characterSwitch

isDiv: 
cmp byte[prevSpecialFlag],0 	
jg  invalidSintax
mov dword[rsi+r15],-83
add r15, 4
inc r10
jmp characterSwitch

isAdd:
cmp byte[prevSpecialFlag],0 	
jg  invalidSintax
inc byte[termCount]
mov byte[operandTermCount],0 	; Reboot operand term counter
cmp byte[termCount],3
jg  invalidSintax
mov dword[rsi+r15],-87
add r15, 4
inc r10
jmp characterSwitch

isSub:
cmp byte[prevSpecialFlag],0 	
jg  invalidSintax
inc byte[termCount]
mov byte[operandTermCount],0 	
cmp byte[termCount],3
jg  invalidSintax
mov dword[rsi+r15],-85
add r15, 4
inc r10
jmp characterSwitch

isCom:
cmp byte[prevSpecialFlag],0 	
jg  invalidSintax
inc byte[dimensionCount]
cmp byte[dimensionCount],3
jg  invalidSintax
mov byte[termCount],0
mov byte[operandTermCount],0
mov dword[rsi+r15],-44
add r15, 4
inc r10
jmp characterSwitch

;***********************************************************************
;							Interval Section 
isOPt:
cmp byte[dimensionCount], 2  	; check if the expression has less than two dimenssions
jl  invalidSintax
cmp byte[prevSpecialFlag], 0 	; check if previous char was a reserved one
jg  invalidSintax
add r11b, byte[variableX]
add r11b, byte[variableY]
cmp r11,0 						; check if there are no variables
je  invalidSintax
;Ojo dividir por 4 ya que r15 tiene cantidad de words
mov r13, r15					; from now on r13 will hold the amount of elements inside the function
xor r15, r15
xor r11, r11
cmp byte[variableX], 0			; check if there are one or two variables
je  oneVariableInterval
cmp byte[variableY], 0
je  oneVariableInterval	
jmp twoVariableInterval


oneVariableInterval:
mov r11, 1						; Amount of variables 
cvtsi2sd xmm0, r11d				; convert from a 32 bit integer into a double precission float number 
movsd [rdx+r15],xmm0			; Add first element of the double array 
add r15,8
inc r10
mov r12b, byte[rdi+r10]			; Check if the number has a minus sign 
cmp r12b, 45
je 	signFlagUpdateOV
cmp r12b, 48					; next character must be a number, is the step first digit.
jl  invalidSintax
cmp r12b, 57
jg  invalidSintax 
push rsi						; process to insert steps in the second space of the float vector 
push rcx
mov rsi, r10
mov rcx, r15
mov r8,1
call floatingPoint
mov r10, rsi 					; updates how many spaces where walked inside the char array
inc byte[intervalXCount]			
add r15,8
inc r10
pop rcx
pop rsi 
jmp oneVariableIntervalLimits


signFlagUpdateOV:
inc r10
mov r12b, byte[rdi+r10]	
cmp r12b, 48
jl  invalidSintax
cmp r12b, 57
jg  invalidSintax 
push rsi						; process to insert steps in the second space of the float vector 
push rcx
mov rsi, r10
mov rcx, r15
mov r8,-1
call floatingPoint
mov r10, rsi					; updates how many spaces where walked inside the char array 
inc byte[intervalXCount] 		; updates how many terms for the intervals have ben processed 
mov r8,1
add r15,8						; moves to the next empty space of the double array 
inc r10
pop rcx
pop rsi 

oneVariableIntervalLimits:
mov r12b, byte[rdi+r10]	
cmp r12, 44						; Check for commas, since it is one variable there can't be commas
je  invalidSintax
cmp r12, 41						; Cheack for the closing parenthesis 
je  intervalDone
cmp r12, 45						; Check if the number has a minus sign 
je signFlagUpdateOV
cmp byte[intervalXCount],7
jg  invalidSintax
push rsi
push rcx						
mov rsi, r10
mov rcx, r15
call floatingPoint
mov r10, rsi
inc byte[intervalXCount]
add r15,8 
inc r10					
pop rcx
pop rsi
jmp oneVariableIntervalLimits


twoVariableInterval:
mov byte[commaPresent], 0		; Default value for commaFlag 
mov r11, 2						; Amount of variables
cvtsi2sd xmm0, r11d				
movsd [rdx+r15],xmm0
add r15,8
inc r10
mov r12b, byte[rdi+r10]			; Check if the number has a minus sign 
cmp r12b, 45
je 	signFlagUpdateTV		
cmp r12b, 48
jl  invalidSintax
cmp r12b, 57
jg  invalidSintax 
push rsi					
push rcx
mov rsi, r10
mov rcx, r15
mov r8, 1
call floatingPoint
mov r10, rsi 					; updates how many spaces where walked inside the char array
inc byte[intervalCount]			
add r15,8
inc r10					
pop rcx
pop rsi 
jmp twoVariableIntervalLimits

signFlagUpdateTV:
inc r10
mov r12b, byte[rdi+r10]	
cmp r12b, 48
jl  invalidSintax
cmp r12b, 57
jg  invalidSintax 
push rsi					
push rcx
mov rsi, r10
mov rcx, r15
mov r8, -1
call floatingPoint
mov r8, 1
mov r10, rsi 					; updates how many spaces where walked inside the char array
inc byte[intervalCount]			
add r15,8
inc r10					
pop rcx
pop rsi 

twoVariableIntervalLimits:
mov r12b, byte[rdi+r10]	
cmp r12, 44
je  correctSpace
cmp r12, 41
je  intervalDone
cmp r12b, 45
je 	signFlagUpdateTV
cmp byte[intervalCount],7
jg  invalidSintax
push rsi
push rcx						
mov rsi, r10
mov rcx, r15
call floatingPoint
mov r10, rsi 					; updates how many spaces where walked inside the char array
inc byte[intervalCount]			
add r15,8
inc r10						
pop rcx
pop rsi
jmp twoVariableIntervalLimits

correctSpace:
inc byte[commaPresent]
inc r10
movsd xmm0, qword[commaCharacter]
movsd [rdx+r15], xmm0
add r15,8
mov r14b,byte[intervalCount]
mov byte[intervalXCount],r14b
mov byte[intervalCount], 0
mov byte[yVariableValidate],1
jmp twoVariableIntervalLimits

intervalDone:
cmp byte[commaPresent], 0
je  invalidSintax
mov r14b, byte[intervalCount]
mov byte[intervalYCount],r14b
mov rax, r13
mov r13, 4
div r13b
jmp intervalRangeValidation 


invalidSintax:
mov rax, -1
jmp return

invalidInterval:
mov rax, -2
jmp return

intervalRangeValidation:
mov   r10,16
xor   r11,r11 
mov   r11b, byte[intervalXCount]
shr   r11,1
xor   r12,r12 
mov   r12b, byte[intervalYCount]
shr   r12,1
xor	  r14, r14
cvtsi2sd xmm4,r14d
cvtsi2sd xmm5,r14d
mov	  rcx, r11 

intervalXLoop:
movsd xmm0, [rdx+r10]
add		r10,8
movsd 	xmm1, [rdx+r10]
add 	r10,8
subsd 	xmm1,xmm0
subsd 	xmm0,xmm0
ucomisd xmm1,xmm0
jb    invalidInterval
addsd   xmm4,xmm1
loop intervalXLoop

mov	  rcx, r12
add r10,8

intervalYLoop:
movsd xmm0, [rdx+r10]
add		r10,8
movsd 	xmm1, [rdx+r10]
add 	r10,8
subsd 	xmm1,xmm0
subsd 	xmm0,xmm0
ucomisd xmm1,xmm0
jb    invalidInterval
addsd   xmm5,xmm1
loop intervalYLoop

combinationAmount:
divsd xmm4,[rdx+8]
divsd xmm5,[rdx+8]
mulsd xmm4,xmm5
mov   r14,1000;
cvtsi2sd xmm0, r14d
ucomisd xmm4, xmm0
ja invalidInterval
jmp return

return:
pop r15 
pop r14
pop r13
pop r12 
pop r11
pop r10 
ret
