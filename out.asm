global _start
_start:
    mov rax, 10
    push rax
    mov rax, 56
    push rax
    pop rax
    pop rbx
    xor  rdx, rdx
    div rbx
    push rdx
    mov rax, 5
    push rax
    mov rax, 2
    push rax
    mov rax, 20
    push rax
    pop rax
    pop rbx
    mul rbx
    push rax
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    mov rax, 2
    push rax
    pop rax
    pop rbx
    mul rbx
    push rax
    pop rax
    pop rbx
    div rbx
    push rax
    mov rax, 10
    push rax
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    mov rax, 4
    push rax
    push QWORD [rsp + 8] ; Variable value

    pop rax
    pop rbx
    add rax, rbx
    push rax
    push QWORD [rsp + 16] ; Variable value

    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    mov rax, 60  ; Syscall number 60 = exit
    mov rdi, 0   ; End program 
    syscall