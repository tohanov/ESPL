global main
global MASK

extern printf

section .data

format: 
	db "%0.4x",
	db 0x0a,		; '\n'
	db 0			; '\0'

MASK: dw 0x002d
STATE: dw 0x5555


section .text
main:
	; call printState

	mov esi, 20
	loopStart:
		mov word bx, 0
		mov word ax, [MASK]
		xor word ax, [STATE]
		jp skipAddOne

		mov bx, 0x8000 		; 0b10...0
		
	skipAddOne:
		shr word [STATE], 1
		add [STATE], bx
		
		call printState

		dec esi
		jnz loopStart

	mov eax, 0 			; return 0
	ret

printState:
	push word 0
	push word [STATE]
	push format
	call printf
	add esp, 8

; p = p(0x5555 ^ 0x002d)
; shr STATE
; if p == 1:
; 	msb(state) = 0

; if p == 0:
; 	msb(state) = 1
