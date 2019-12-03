; yasm -f elf64 -g dwarf2 VFunction.asm -o VFunction.o
; gcc -no-pie -o funxd fun.o main.c
; https://en.wikibooks.org/wiki/X86_Assembly/SSE
; getAxisValues(&function, &intervals, &results);
;
; rdi:	function 	int array
; rsi:	intervals	double array
; rdx:	results		double array	

extern printf	
			
section .data
	formatoDouble db "%.2f ", 0
	formatoEntero db "%d ",10, 0
	printFunction db "Function: %c ",10, 0
	printFunctionNum db "Function: %d ",10, 0
	printVar db "Cuartil: %d ",10, 0
		
	showEpsilon db "Epsilon: %.2f ",10, 0
	showIntervaloX db "Intervalo X: %.2f ",10, 0
	showIntervaloY db "Intervalo Y: %.2f ",10, 0
	
	showIntervaloA db "Intervalo A: %.2f ",10, 0
	showIntervaloP db "Intervalo P: %.2f ",10, 0
	
	showIntervaloB db "Intervalo B: %.2f ",10, 0
	showIntervaloQ db "Intervalo Q: %.2f ",10, 0
	showX db "Valor X: %.2f ",10, 0
	showY db "Valor Y: %.2f",10, 0

	epsilon dq 0     
	valorX dq 0
	valorY dq 0
	
	intervalA dq 0.0 ; intervalo inicial para X
	intervalB dq 0.0 ; intervalo final para X
		
	intervalP dq 0.0 ; intervalo inicial para Y
	intervalQ dq 0.0 ; intervalo final para Y
	
	esX dq 120
	esY dq 121
	esSuma dq 43
	esMul dq 42
	esDiv dq 47; valor ASCII de / -130
	esSen dq 115 ; 115-130
	esCos dq 99
	esTan dq 116
	esComa dq -86
	esComaD dq 44
	
	contadorVariablesQuartiles dq 0, 0, 0, 0, 0, 0, 0 ,0 ,0
	
    v1 dd 1.1, 2.2, 3.3, 4.4    ;first set of 4 numbers
    v2 dd 5.5, 6.6, 7.7, 8.8   ;second set
	
	numeroADividir db 1
	
section .bss
	intervalsX resq 7
	intervalsY resq 7
	
	
	; se usaran para las instrucciones con avx
	vector1 resq 4	; 
	vector2 resq 4
	vector3 resq 4
	vector4 resq 4
	
	vectorRes1 resq 4 
	vectorRes2 resq 4
	vectorRes3 resq 4
	vectorRes4 resq 4
	
	valorSen resq 1
	valorCos resq 1
	valorTan resq 1
	
	
	v3 resd 4
	
section .text

global getAxisValues			
getAxisValues:

	push rbx
	push rbp
	push r11		; guardar valor de function
	push r12		; puntero a function
	push r13		; puntero a intervalos
	push r14		; contador general
	push r15		; contador del arreglo intervals
	
	xor r11, r11
	xor r12, r12
	xor r13, r13
	xor r14, r14
	xor r15, r15
	
	mov r13, rdi		; puntero a function
	mov r12, rsi		; puntero a intervals

	xor rsi, rsi
	
	
    movups xmm0, [v1]   ;load v1 into xmm0
    movups xmm1, [v2]   ;load v2 into xmm1
    addps xmm0, xmm1 
    
    movups [v3], xmm0   ;store v1 in v3
    
    ciclo1:
		
	movsd xmm1, qword [v3+(r14*4)] ; muevo el valor del epsilon
	;movsd qword[epsilon], xmm1
	
	mov	rdi, formatoDouble
	movsd xmm0, xmm1
	mov rax, 1
	call printf
		
	inc r14
	cmp r14,5
	jne ciclo1
		
	jmp last

	movsd xmm1, qword [r12+(r14*4)] ; muevo el valor del epsilon
	movsd qword[epsilon], xmm1
	
	mov	rdi, showEpsilon
	movsd xmm0, qword[epsilon]
	mov rax, 1
	call printf
	
	inc r15
	
saveIntervalsX:	
	
	cvtsd2si eax, qword [r12+(r15*8)]
	cmp eax, dword [esComa]
	je nextSave
	
	movsd xmm1, qword [r12+(r15*8)]	; r12 apunta al array de intervals
	movsd qword [intervalsX+(r14*8)], xmm1	; empeiza en 0
	
	;cvtsi2sd xmm1, qword [esComa]
	
	mov	rdi, showIntervaloX
	movsd xmm0, qword [intervalsX+(r14*8)]
	movsd xmm0, xmm1
	mov rax, 1
	call printf
	
	inc r14
	inc r15
	
	jmp saveIntervalsX
	
nextSave:
	xor rax, rax
	xor r14,r14
	inc r15
	
saveIntervalsY:

	cvtsd2si eax, qword [r12+(r15*8)]
	cmp eax, byte 0
	je finishSave
	
	movsd xmm1, qword [r12+(r15*8)]
	movsd qword [intervalsY+(r14*8)], xmm1
	
	mov	rdi, showIntervaloY
	movsd xmm0, qword [intervalsY+(r14*8)]
	mov rax, 1
	call printf
	
	inc r14
	inc r15
		
	jmp saveIntervalsY
	
finishSave:	
	xor r8, r8	; contador de x's
	xor r9, r9	; contador de y's
	xor rax, rax
	xor r14,r14 ; limpiamos contadores
	xor r15,r15 ; contador de comas
	
getVariables:	; r13 apunte al arreglo de function
	mov rbx, qword [r13+(r14*4)]
	
	cmp ebx, byte 0
	jge esNumero
	add rbx, 130

	cmp ebx, dword[esComaD]
	je saveDatosQuartil
	
	cmp ebx, dword[esSuma]
	je saveDatosQuartil
	
	cmp ebx, dword [esX]
	je foundX
	
	cmp ebx, dword [esY]
	je foundY
	
	esNumero:
	
	inc r14
	cmp r14, 198
	jge saveDatosQuartil
	
	jmp getVariables
	
	foundX:
		inc r8	;contador de x's
		inc r14
		jmp getVariables
	
	foundY:
		inc r9 ; contador de y's
		inc r14
		jmp getVariables
	
	
saveDatosQuartil:
	cmp r8, byte 0
	jne tieneX
	
	continuarSaveDatos1:
	
	cmp r9, byte 0
	jne tieneY
	
	continuarSaveDatos2:
	
	inc r15
	inc r14
	
	xor r8,r8
	xor r9,r9
	
	cmp r14, 198
	jge	finishGetVariables
	cmp r15, 9
	jge finishGetVariables
	
	jmp getVariables
	
	tieneX:
		add qword[ contadorVariablesQuartiles+(r15*4) ], 88
		xor r8, r8
		jmp continuarSaveDatos1
		
	tieneY
		add qword[ contadorVariablesQuartiles+(r15*4) ], 44
		xor r9, r9
		jmp continuarSaveDatos2
	
	
finishGetVariables:
	xor r14,r14
	xor r15, r15
	
	ciclo:
		mov rax, qword[contadorVariablesQuartiles+(r14*4)]
		
		mov	rdi, printVar
		mov rsi, rax
		xor rax, rax
		call printf
		
		inc r14
		cmp r14, 9
		jne ciclo
		
		xor r14, r14
	
asignIntervals:
	movsd xmm1, qword [intervalsX+(r14*8)]
	movsd qword[intervalA], xmm1
	
	mov	rdi, showIntervaloA
	movsd xmm0, xmm1
	mov rax, 1
	call printf
	
	movsd xmm1, qword [intervalsY+(r14*8)]
	movsd qword[intervalP], xmm1
	
	mov	rdi, showIntervaloP
	movsd xmm0, xmm1
	mov rax, 1
	call printf
	
	inc r14
	
	movsd xmm1, qword [intervalsX+(r14*8)]
	movsd qword[intervalB], xmm1
	
	mov	rdi, showIntervaloB
	movsd xmm0, xmm1
	mov rax, 1
	call printf
	
	movsd xmm1, qword [intervalsY+(r14*8)]
	movsd qword[intervalQ], xmm1
	
	mov	rdi, showIntervaloQ
	movsd xmm0, xmm1
	mov rax, 1
	call printf

	
asignVariables:	;aqui tendría que hacer las comparaciones para saber en que rango estoy y todo eso
	movsd xmm0, qword[intervalA]
	movsd qword [valorX], xmm0
	
	movsd xmm0, qword[intervalP]
	movsd qword [valorY], xmm0
	
	xor rbx, rbx	; lleva cuenta de 0s
	xor rbp, rbp	; lleva cuenta de los +
	xor rcx, rcx	; lleva cuenta para la pos de los valores en los arrays
	xor r8, r8		; without an use for now
	xor r9, r9		; same
	xor r14, r14	; limpio contadores
	xor r15, r15	; limpio contadores 
	
	
	jmp last
ejeX:

ejeY:

ejeZ	
	


recorridoFunction: ; r13 apunta al array functions
	mov rbx, qword [r13+(r14*4)]
	
	cmp rax, byte 0
	jae esConstante
	
	cmp rax, qword [esX]
	je encontreX
	
	cmp rax, qword [esY]
	je encontreY
	
	jmp esOperador
	
	
	continuar:
		inc r14			; contador para movernos en el arreglo function
		inc rcx			; contador de posiciones de los cuartiles
		
		cmp rbx, byte 3			; comparo con el contador de 0's para saber en qué eje estoy
		jne ejecutarParallel
		
		cmp rbp, byte 2		; comparo con el contador de +'s para saber en qué cuartil del eje estoy
		jne ejecutarParallel
		
		cmp rcx, byte 1		; comparo con el contador de posiciones para saber si estoy en el primer o segundo dato del cuartil
		jne ejecutarParallel
		
		;jmp ejeZ ; termine
		
		
		ejecutarParallel:
			cmp rbx,2
			;je ejeX
				
			cmp rbx, 3
			;je ejeY
		
			jmp recorridoFunction
		
esConstante:
	cmp rax, byte 0
	je encontreCero
	
	cvtsi2sd xmm0, dword [eax]
	jmp guardarEnVector
	
	;cmp rcx, byte 3 
	
	encontreCero:
		inc rbx ; contador de 0s para los ejes
		jmp continuar

encontreX:
	movsd xmm0, qword [valorX]	
	jmp guardarEnVector
	
encontreY:
	movsd xmm0, qword [valorY]
	jmp guardarEnVector

esOperador:
	cmp rax, qword [esDiv]
	je convertirAMul
	
	
	cmp rax, qword[esSen]
	je encontreSen
	
	cmp rax, qword[esCos]
	je encontreCos
	
	cmp rax, qword[esTan]
	je encontreTan
	
	jmp continuar
	
	
	encontreSen:
		inc r14
		mov rax, qword [r11+(r14*4)]
		
		cmp rax, byte 0
		jae esConstanteSen
		
		
		esConstanteSen:
			fld qword[rax]
			fsin
			fst qword[valorSen]
		
		;~ cvtsi2sd xmm0, dword [eax]
		movsd xmm0, qword[valorSen]
		jmp guardarEnVector
		
		
	encontreCos:
		inc r14
		mov rax, qword [r11+(r14*4)]
		
		cmp rax, byte 0
		jae esConstanteCos
		
		esConstanteCos:
			fld qword[rax]
			fcos
			fst qword[valorCos]
		
		movsd xmm0, qword[valorCos]
		jmp guardarEnVector	
			
	
	encontreTan:
		inc r14
		mov rax, qword [r11+(r14*4)]
		
		cmp rax, byte 0
		jae esConstanteTan
		
		esConstanteTan:
			fld qword[rax]
			fptab
			fst qword[valorTan]
	
		movsd xmm0, qword[valorTan]
		jmp guardarEnVector	
		
convertirAMul:
	inc r14
	mov rax, qword [r11+(r14*4)]
		
	cmp rax, byte 0
	jae esConstanteAConvertir
	
	cmp rax, qword [esX]
	je encontreX
	
	cmp rax, qword [esY]
	je encontreY
		
	esConstanteAConvertir:
		cvtsi2sd xmm0, dword [numeroADividir]
		cvtsi2sd xmm3, dword [eax]
		divsd xmm0, xmm3	
		jmp guardarEnVector
		
	; resetear rcx cada que se encuentra +

guardarEnVector:
	cmp rcx, byte 0	; para saber si estoy en el primer dato o segundo del cuartil
	je primerVector
	
	segundoVector:	
		movsd [vector2+(rbp*8)],xmm0
		jmp continuar

	primerVector:
		movsd [vector1+(rbp*8)],xmm0
		jmp continuar


	
guardarEnResults: ; r13 tiene el puntero a results	
	jmp last
		

last:
	pop r15			; recuperamos registros
	pop r14			; recuperamos registros
	pop r13			; recuperamos registros
	pop r12			; recuperamos registros
	pop r11
	pop rbp
	pop rbx
	;add rsp, 64
	;pop rbp			; regresamos a la dir de retorno
	
	ret				; nos salimos del programa
