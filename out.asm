global _start
_start:
    ;; /set
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
    ;; /set
    mov rax, 1
    push rax
    ;; /if
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label0
    ;; /reset
    mov rax, 1
    push rax
    pop rax
    mov [rsp + 8], rax
    add rsp, 0
    jmp label1
label0:
    ;; /elseif
    mov rax, 0
    push rax
    pop rax
    test rax, rax
    jz label2
    ;; /reset
    mov rax, 2
    push rax
    pop rax
    mov [rsp + 8], rax
    add rsp, 0
    jmp label1
label2:
    ;; /else
    ;; /reset
    mov rax, 3
    push rax
    pop rax
    mov [rsp + 8], rax
    add rsp, 0
label1:
    ;; /if
    ;; /exit
    push QWORD [rsp + 8] ; Variable value
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    mov rax, 60  ; Syscall number 60 = exit
    xor rdi, rdi ; End program with 0
    syscall