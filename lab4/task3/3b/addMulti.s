global add_multi
global main

extern printf
extern puts
extern malloc
extern free


format: 
	db "%0.2x",
	; db 0x0a,		; '\n'
	emptyString:
		db 0			; '\0'

plus: db "+", 0
equals: db "=", 0

x_struct: dd 5
	x_num: db 0xaa, 1, 2, 0x44, 0x4f
y_struct: dd 6
	y_num: db 0xaa, 1, 2, 3, 0x44, 0x4f


main:
	push y_struct	; arg 1
	call print_multi	; print_multi(y_struct)
	add esp, 4

	push plus
	call puts
	add esp, 4

	push x_struct	; arg 1
	call print_multi	; print_multi(x_struct)
	add esp, 4

	push equals
	call puts
	add esp, 4

	push x_struct	; arg 2
	push y_struct	; arg 1
	call add_multi	; add_multi(y_struct, x_struct)

	; clean args
	add esp, 8

	push eax
	call print_multi

	; eax still holds the pointer
	call free

	; clean args
	add esp, 4

	; return 0
	mov eax, 0
	ret


; struct multi* add_multi(struct multi *p, *q)
; assumes p->len greater than q->len
add_multi:
	push ebp
	mov ebp, esp

	sub esp, 8		; declare 2 vars on the stack

	mov dword ebx, [ebp + 8]	; ebx points to struct1
	mov ebx, [ebx]		; len1
	mov [ebp - 4], ebx		; VAR1 = len1

	add ebx, 4			; calculating sizeof(struct1)
	push ebx			; arg1 = sizeof(struct1)
	call malloc			; malloc(sizeof(struct1))
	; cleaning arg1 from stack
	add esp, 4

	mov ebx, [ebp - 4]	; ebx = VAR1 = len1
	mov [eax], ebx		; newStruct.len = ebx = len1

	mov dword ebx, [ebp + 12]	; ebx points to struct2
	mov ebx, [ebx]				; ebx = len2
	sub dword [ebp - 4], ebx	; VAR1 = len1 - len2
	mov dword [ebp - 8], ebx	; VAR2 = len2
	

	add eax, 4			; eax = newStruct.num
	
	mov ebx, [ebp + 8]
	add ebx, 4			; ebx = struct1.num
	mov ecx, [ebp + 12]
	add ecx, 4			; ebx = struct2.num

	mov esi, 0			; esi = 0 (counter)
	mov edi, [ebp - 8] 	; esi = var2 = len2
	; inc edi

	and al, al	; clear CF

	forLoop1:
		; dec edi
		; jz end1

		mov byte dl, [ebx + esi]
		adc byte dl, [ecx + esi]
		mov byte [eax + esi], dl

		; pushad
		; push eax
		; sub dword [esp], 4
		; call print_multi
		; add esp, 4
		; popad

		inc esi
		dec edi
		jnz forLoop1
	end1:

	mov edi, [ebp - 4]	; edi = var1 = len1 - len2
	; inc edi
	forLoop2:
		mov byte dl, [ebx + esi]
		adc byte dl, 0
		mov byte [eax + esi], dl

		; pushad
		; push eax
		; sub dword [esp], 4
		; call print_multi
		; add esp, 4
		; popad

		inc esi
		dec edi
		jnz forLoop2
		; jmp forLoop2
	end2:
	
	sub eax, 4 ; eax = &newStruct
	add esp, 8 ; erase VAR1, VAR2
	
	pop ebp
	ret


; print_multi(struct multi *p))
print_multi:
	push ebp		; preserve previous ebp
	mov ebp, esp	; init new function-relative ebp
	add ebp, 4		; take into account the push of previous ebp

	; value of pointer to struct goes in esi
	; this is also the pointer to the size field
	mov esi, [ebp + 4]	; esi points to size

	; making edi point to just after the end of byte array
	mov edi, esi		; edi now points to struct
	add edi, 4			; edi point to beginning of array
	add edi, [esi]		; edi points to after the end of the array

	; make esi point to beginning of byte array
	add esi, 4

	forLoop:
		dec edi
		cmp edi, esi
		jl loopEnd		; if edi >= esi, then enter loop
		
		mov eax, [edi]	; get a word with the needed byte
		and eax, 0xFF	; isolate the byte
		push eax		; second argument: (char)(*(edi))
		push format		; first argument: const char* format
		; printf(format, *(esi))
		call printf

		;clean stack from arguments
		add esp, 8

		jmp forLoop

	loopEnd:
		call printNewline

	pop ebp			; recover previous ebp before going back
	ret

	
; printNewline:
; 	push emptyString
; 	call puts
; 	; clean stack
; 	add esp, 4

; 	ret
