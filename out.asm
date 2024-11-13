section .data
    char db 0    ;; Allocate byte for a char

section .text
    global _start

_start:
    ;; /init list
    mov rax, 1
    push rax
    mov rax, 0
    push rax
    mov rax, 0
    push rax
    ;; /set
    mov rax, 10
    push rax
    ;; /set
    mov rax, 0
    push rax
    ;; /while
label0:
    push QWORD [rsp + 8] ; Variable value
    push QWORD [rsp + 8] ; Variable value
    pop rax
    pop rbx
    xor rcx, rcx
    cmp rax, rbx
    setl cl
    push rcx
    pop rax
    test rax, rax
    jz label1
    ;; /array location
    ;; /array location
    mov rax, 1
    push rax
    mov rax, rsp
    add rax, 40
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    ;; /array location
    mov rax, 0
    push rax
    mov rax, rsp
    add rax, 48
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    pop rax
    pop rbx
    add rax, rbx
    push rax
    mov rax, 2
    push rax
    mov rcx, rsp
    add rcx, 48
    pop rbx
    shl rbx, 3
    sub rcx, rbx
    pop rax
    mov [rcx], rax
    ;; /array location
    ;; /array location
    mov rax, 1
    push rax
    mov rax, rsp
    add rax, 40
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    mov rax, 0
    push rax
    mov rcx, rsp
    add rcx, 48
    pop rbx
    shl rbx, 3
    sub rcx, rbx
    pop rax
    mov [rcx], rax
    ;; /array location
    ;; /array location
    mov rax, 2
    push rax
    mov rax, rsp
    add rax, 40
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    mov rax, 1
    push rax
    mov rcx, rsp
    add rcx, 48
    pop rbx
    shl rbx, 3
    sub rcx, rbx
    pop rax
    mov [rcx], rax
    ;; /reset
    mov rax, 1
    push rax
    push QWORD [rsp + 8] ; Variable value
    pop rax
    pop rbx
    add rax, rbx
    mov [rsp + 0], rax
    add rsp, 0
    jmp label0
label1:
    ;; /while
    ;; /if
    mov rax, 55
    push rax
    ;; /array location
    mov rax, 2
    push rax
    mov rax, rsp
    add rax, 48
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    pop rax
    pop rbx
    xor rcx, rcx
    cmp rax, rbx
    sete cl
    push rcx
    pop rax
    test rax, rax
    jz label2
    ;; /print
    mov rax, 112
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 97
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 115
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 115
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 10
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    add rsp, 0
    jmp label3
label2:
    ;; /else
    ;; /print
    mov rax, 102
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 97
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 105
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 108
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 10
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    add rsp, 0
label3:
    ;; /if
    ;; /end

    mov rax, 60  ; Syscall number 60 = exit
    xor rdi, rdi ; End program with 0
    syscall