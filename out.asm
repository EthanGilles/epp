global _start
_start:
    mov rax, 2
    push rax
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    mov rax, 9
    push rax
    mov rax, 10
    push rax
    push QWORD [rsp + 8]

    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    mov rax, 60  ; Syscall number 60 = exit
    mov rdi, 0   ; Exit value 
    syscall