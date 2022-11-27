segment .data
Msg db 0xA
   lenMsg equ $-Msg        
forstr db      "%s", 0x0a, 0
forint db      "%d", 10, 0
        segment .text
        global  main            ; let the linker know about main
        ;extern  printf,puts          ; resolve printf from libc
main:   push    ebp             ; prepare stack frame for main
        mov     ebp, esp
        pushad
        sub     esp, 8
        mov     edi, dword[ebp+8]    ; get argc into edi
        mov     esi, dword[ebp+12]   ; get first argv string into esi
print_argc: ;xor     eax, eax
            ;push    dword edi ; must dereference edi; points to argc
           ; push    forint
            ;call    printf
           ; mov	edx,4		; message length
           ; mov	ecx,dword edi		; message to write
           ; mov	ebx,1		; file descriptor (stdout)
           ; mov	eax,4		; system call number (sys_write)
           ; int	0x80		; call kernel
           ; mov eax, 4
           ; mov ebx, 1
           ; mov ecx, Msg
           ; mov edx, lenMsg
            ;int 80h  

print_argv: xor     eax, eax
            mov	ecx,dword [esi] ; must dereference esi; points to argv
            call strlen
            mov	ecx,dword [esi]
            mov edx, eax
            mov	ebx,1		; file descriptor (stdout)
            mov	eax,4		; system call number (sys_write)
            int	0x80		; call kernel
            xor eax, eax 
            mov eax, 4
            mov ebx, 1
            mov ecx, Msg
            mov edx, lenMsg
            int	0x80 
            add     esi, 4          ; advance to the next pointer in argv
            dec     edi             ; decrement edi from argc to 0
            cmp     edi, 0          ; when it hits 0, we're done
            jnz     print_argv     ; end with NULL pointer

after_print_argv: xor     eax, eax
                  mov     esp , ebp
                  pop     ebp
                  ret
strlen:mov eax,1
count:cmp byte[ecx],0
      jz done
      inc ecx
      inc eax
      jmp count
done: ret