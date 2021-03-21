.code

; Bufor z pikselami, prerabiana cze��, wysoko��, szeroko��, start, koniec
Main proc pixelBuffer: QWORD, part: QWORD, higth: SDWORD, wid: SDWORD, start: SDWORD, endIndex: SDWORD

	; Odtw�rz rejestr
	push rbx
	push rcx
	push rdx
	push rbp

	; Pocz�tek jest iteratorem w rejestrze rsi
	mov eax, start
	mov rsi, rax

	; Wysoko�� w rejestrze r12 
	mov r12, r8

	; Width * hight * 3 w r12
	imul r12, r9
	imul r12, 3

	; Width * 3 - 3 w r9
	imul r9, 3
	sub r9, 3

	; Width * 3 + 3 w rdi
	mov rdi, r9
	add rdi, 6

	; Indeks dla bufora w rejestrze r14
	xor r14, r14
	mov eax, endIndex
	sub eax, start
	mov r14, rax

	; Sprawd� czy jest to ostatni w�tek
	mov eax, endIndex
	cmp rax, r12
	je LastThread
	add r14, rdi
	LastThread:
	sub r14, rdi

	; Indeks dla obliczanej cz�ci w r13
	xor r13, r13

	; Dzielnik
	mov r15, 8
	movd xmm10, r15

	; Dzielna
	mov r15, 1
	movd xmm9, r15

	; Konwersja na zmiennoprzecinkowe
	vcvtdq2ps xmm10, xmm10
	; Konwersja na zmiennoprzecionkowe
	vcvtdq2ps xmm9, xmm9

	; Podzielic przez 8 oznacza pomnozyc przez 0.125
	divss xmm9, xmm10
	; Ustaw 0.125 na wszystkich miejscach
	shufps xmm9, xmm9, 0

	; Sprawd� czy jest to pierwszy w�tek
	mov rax, 0
	cmp rax, rsi
	jne NotFirstThread
	add rsi, r9
	NotFirstThread:

	ForLoop:
		
		; Sprawd� czy indeks nowej bitmapy jest mniejszy od konca
		cmp r13, r14
		jge ProgramOver

		; Przesu� wska�nik
		mov rbx, rsi
		sub rbx, r9

		; 1 zestaw pikseli
		; Indeks i - width * 3 - 3
		pmovzxbd xmm0, [rcx + rbx - 6]
		; Rozkaz pmovzxbd (SSE4_1):
		; Przenosi 4 16-bitowe warto�ci z bufora do xmm 

		; 2 zestaw pikseli
		; Indeks i - width * 3
		pmovzxbd xmm2, [rcx + rbx - 3]
		; Sumuj warto�ci
		paddd xmm0, xmm2 
		; Rozkaz paddd
		; Dodawanie warto�ci dw�ch wektor�w
		; Np: xmm0: 321 123 321 100
		;	  xmm1: 111 111 111 111
		;     --------------------- paddd xmm0, xmm1
		;     xmm0: 432 234 432 211

		; 3 zestaw pikseli
		; Indeks i - width * 3 + 3
		pmovzxbd xmm2, [rcx + rbx]
		; Sumuj warto�ci
		paddd xmm0, xmm2 
		; 4 zestaw pikseli
		; Indeks i - 3
		pmovzxbd xmm2, [rcx + rsi - 3]
		; Sumuj warto�ci
		paddd xmm0, xmm2
		; 5 zestaw pikseli
		; Indeks i + 3
		pmovzxbd xmm2, [rcx + rsi + 3]
		; Sumuj warto�ci
		paddd xmm0, xmm2
		; Przesu� wska�nik
		mov rbx, rsi
		add rbx, r9
		; 6 zestaw pikseli
		; Indeks i + width * 3 - 3
		pmovzxbd xmm2, [rcx + rbx - 3]
		; Sumuj warto�ci
		paddd xmm0, xmm2
		; 7 zestaw pikseli
		; Indeks i + width * 3
		pmovzxbd xmm2, [rcx + rbx]
		; Sumuj warto�ci
		paddd xmm0, xmm2
		; 8 zestaw pikseli
		; Indeks i + width * 3 + 3
		pmovzxbd xmm2, [rcx + rbx + 3]
		; Sumuj warto�ci
		paddd xmm0, xmm2 
		
		; Pomn� wektor przez 0.125
		mulps xmm0, xmm9

		; Konwersja na zmiennoprzecinkowe
		vcvtdq2ps xmm1, xmm0
		; Zapisz wynik w rax
		vcvtss2si eax, xmm1
		
		; Rozkaz vcvtss2si (AVX)
		; Przemie�� najmniej znacz�cy element zmiennoprzecinowy z xmm0,
		; konwertuj�c go na 32bitowy integer (dlatego eax a nie rax)
		; Zapisz obliczony piksel w nowej bitmapie
		mov byte ptr [rdx + r13], al
		; Inkrementuj indeks nowej bitmapy
		inc r13
		; Przesu�
		shufps xmm0, xmm0, 39h
		; Rozkaz shufps (SSE)
		; Rozkaz przesuwa dane w wektorze w zale�no�ci od flagi kontolnej
		; 39h - przesu� w prawo
		; Konwersja na zmiennoprzecinkowe
		vcvtdq2ps xmm1, xmm0
		; Prze�lij do eax
		vcvtss2si eax, xmm1
		; Zapisz w nowej bitmapie
		mov byte ptr [rdx + r13], al
		; Inkrementuj indeks
		inc r13
		; Przesu�
		shufps xmm0, xmm0, 39h
		; Konwersja na zmiennoprzecinkowe
		vcvtdq2ps xmm1, xmm0
		; Prze�lij do eax
		vcvtss2si eax, xmm1
		; Zapisz w nowej bitmapie
		mov byte ptr [rdx + r13], al
		; Inkrementuj indeks
		inc r13
		
		add rsi, 3
		jmp ForLoop

ProgramOver:
	pop rbp
	pop rdx
	pop rcx
	pop rbx

	ret
	Main endp
    end