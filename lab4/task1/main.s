global main
extern printf
extern puts


section .rodata
format: db "%d", 0x0a, 0


section .text
main: 							; main(int argc, char **argv)
	push ebp
	mov ebp, esp

	push dword [ebp + 8]		; push argc to stack
	push dword format			; push format string pointer to stack
	call printf					; printf(format, argc)
	; clean stack after call
	add esp, 8

	call printArgv

	pop ebp
	mov eax, 0
	ret


printArgv:
	mov esi, 0						; i = 0

	myloop:
		mov eax, [ebp + 12]			; re-init eax to argv
		push dword [eax + 4*esi]	; passing *(argv + i) as argument
		call puts 					; puts(argv[i])
		; clean stack after call
		add esp, 4

		inc esi
		cmp esi, [ebp + 8]			; compare i to argc

		jne myloop					; keep looping while i < argc

	ret