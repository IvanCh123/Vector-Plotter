;Converts from characters expresions into float numbers 

;***********************************************************************
;***********************************************************************
;								DATA SECTION
section .data
fractionalDigits db 0 ; Digits after the point of a number 

;***********************************************************************
;***********************************************************************
; 								BSS SECTION
section .bss


;***********************************************************************
;***********************************************************************
; 								CODE SECTION
;First->	rdi:	charVectorFunction	char array
;Second->	rsi:	init       			position to begin in charVectorFunction
;Third->	rdx:	interval array 		float array
;Forth->	rcx: 	insert				position to insert in interval array
;Fifth->	r8:		sign flag			determine the sign of the final number
section .text
global floatingPoint
floatingPoint:

push r11
push r12
push r13

xor  r11,r11					; Place where digits fuse
;mov r12b, byte[rdi+rdx]
;sub r12b
;mov r11, r12
;inc rdx

leftCommaProcessor:
mov r12b, byte[rdi+rsi]
cmp r12b, 46					; Check if it is a point
je  rightCommaProcessor			; The first iteration for sure wont be a point
cmp r12b, 32					; Check if it is a space
je  floatingConvertion			; The first iteration for sure wont be a space
mov r13,r11
shl r11, 3						; multiply by 8
shl r13, 1						; multiply by 2
add r11,r13						; r11*8+r11*2 = r11*10
sub r12,48
add r11,r12					    ; 52*8 + 52*2 + 3 = 523 // r11=52 r12=3 
inc rsi
jmp leftCommaProcessor		

rightCommaProcessor:
inc rsi
mov r12b, byte[rdi+rsi]
cmp r12b, 32
je  floatingConvertion
inc byte[fractionalDigits] 	
mov r13,r11
shl r11, 3						; multiply by 8
shl r13, 1						; multiply by 2
add r11,r13						; r11*8+r11*2 = r11*10
sub r12,48
add r11,r12					    ; 52*8 + 52*2 + 3 = 523 // r11=52 r12=3 
jmp rightCommaProcessor		

floatingConvertion:
cvtsi2sd xmm0, r11d				
mov r11,10
cvtsi2sd xmm1, r11d
xor r11, r11

divLoop:
cmp byte[fractionalDigits],0
je  insertFloatNumber
divsd xmm0, xmm1				; divide two 64 float numbers 
dec byte[fractionalDigits]
jmp divLoop

insertFloatNumber:
cvtsi2sd xmm2,r8
mulsd xmm0,xmm2					;r8 contains the sign of the number
movsd [rdx+rcx], xmm0
pop r13
pop r12
pop r11
ret


