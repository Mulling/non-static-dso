BITS 64

in:
    mov eax, 1 ; write
    mov edi, 1 ; stdout
    mov rsi, 0xFFFFFFFFFFFFFFFF ; <msg addr> FIXME: not generating the right instruction
    mov edx, 0xFFFFFFFF ; <msg len>
    syscall
    mov eax, 35 ; nanosleep
    mov rdi, 0xFFFFFFFFFFFFFFFF  ; <tv addr> FIXME: not generating the right instruction
    mov rsi, 0 ; (NULL)
    syscall
    jmp in
