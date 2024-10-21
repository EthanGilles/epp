global _start
_start:
    mov rax, 7
    push rax
    mov rax, 8
    push rax
    mov rax, 9
    push rax
    push QWORD [rsp + 0]

    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    mov rax, 60  ; Syscall number 60 = exit
    mov rdi, 0   ; Exit value 
    syscall
