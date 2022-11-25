global main

extern printf
extern puts

; struct multi {
; 	unsigned int size;
; 	unsigned char num [ ]
; }
section .rodata
x_struct:
	dd 5 					; size with value 5
	x_num:
		; {aa, 01, 02, 4f} representing 0x4f0201aa
		db 0xaa, 1, 2, 0x44, 0x4f

format: 
	db "%0.2x",
	; db 0x0a,		; '\n'
	emptyString:
		db 0			; '\0'


section .text
main:
	push x_struct
	call print_multi
	add esp, 4		; clean stack
	
	mov eax, 0		; return 0
	ret


; print_multi(struct multi *p))
print_multi:
	push ebp		; preserve previous ebp
	mov ebp, esp	; create new relative ebp
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

	
printNewline:
	push emptyString
	call puts
	; clean stack
	add esp, 4

	ret