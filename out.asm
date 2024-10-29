global _start
_start:
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
    mov rax, 10
    push rax
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    pop rbx
    div rbx
    push rax
    mov rax, 1
    push rax
    mov rax, 1
    push rax
    push QWORD [rsp + 8] ; Variable value
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    test rax, rax
    jz label0
    mov rax, 69
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    add rsp, 0
label0:
    mov rax, 1
    push rax
    push QWORD [rsp + 8] ; Variable value
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    test rax, rax
    jz label2
    mov rax, 68
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    add rsp, 0
    jmp label1
label2:
    mov rax, 67
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    add rsp, 0
label1:
    mov rax, 60  ; Syscall number 60 = exit
    mov rdi, 0   ; End program 
    syscall