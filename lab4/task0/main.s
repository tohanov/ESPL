global main
extern printf
extern puts

section .rodata
format: db "%d", 0x0a, 0		; string bytes, newline, zero-terminator

section .text
main: 							; main(int argc, char **argv)
	push ebp
	mov ebp, esp
	
	push dword [ebp + 8]		; arg2 = *(&argc)
	push dword format			; arg1 = format string address
	call printf					; printf(format, argc)
	
	; clean stack after call
	add esp, 8

	mov eax, [ebp + 12]			; eax = argv
	push dword [eax]			; push *argv
	call puts 					; puts(argv[0])

	; clean stack after call
	add esp, 4

	pop ebp
	
	mov eax, 0
	ret