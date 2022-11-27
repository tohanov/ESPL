section .data
      my_e: db '+e',0xa,0
section .bss
    enc_key: resd 1
    enc_flag: resb 1
    char1: resb 1
global main

section .text

main:
    push    ebp
    mov     ebp, esp
    ;pushad
    ;sub     esp, 8
    mov     edi, [ebp+8] ; pointer to argc
    mov     esi, [ebp+12] ;pointer to the argv[]  
    mov     byte[enc_flag], -1 ;int enc_flag = 0
loop_argv:
     cmp     edi, 0          ; check if 0 then end
     jle      get_char 
     mov     ebx,[esi] ; dereference, take argument 
     add     esi, 4
     dec     edi  
     cmp     word[ebx], "+e" ;check if argv is + 
     je      save_enc ;if equal save it
     jmp     loop_argv  ;else scan argv again
save_enc:
    add     ebx,2 ; dont need +e
    mov     dword[enc_key], ebx ; enc_key = 123 (without +e)
    mov     byte[enc_flag], 0 ;enc_flag = 0
    ;---------------test--------------
    ; pushad
    ; mov eax, 4              ; write
    ; mov ebx, 1              ; stdout
    ; mov ecx, my_e       ; put char
    ; mov edx, 4              ; length
    ; int 0x80
    ; popad
    ;---------------end test-----------
    jmp     loop_argv   
get_char:
    pushad
    mov eax, 3              ; read
    mov ebx, 0              ; stdin
    mov ecx, char1          ; save char inside char1
    mov edx, 1              ; length 
    int 0x80
    cmp eax, 0
    jle exit_program ;if there is nothing to read then exit 
    popad
    cmp byte[char1], 0x0a 
    je reset_edi    
    cmp byte[enc_flag], 0 
    je encrypt_char
end_get_char:
    jmp print_char
reset_edi:
    mov edi, 0
    jmp end_get_char

encrypt_char:
    mov ecx, dword[enc_key]
    mov bl, byte[ecx+edi]   
    sub bl, '0'       ; char to int
    add byte[char1], bl
    add edi, 1
    cmp byte[ecx+edi], 0
    je conter_to_zero
    jmp print_char
conter_to_zero:
    mov edi, 0
print_char:
    pushad
    mov eax, 4              ; write
    mov ebx, 1              ; stdout
    mov ecx, char1         ; put char
    mov edx, 1              ; length
    int 0x80
    popad
    jmp get_char

exit_program:
    popad
    mov esp , ebp
    pop ebp
    ret

    