section .data
    char db 0    ;; Allocate byte for a char

section .text
    global _start

_start:
    ;; /init list
    mov rax, 65
    push rax
    mov rax, 66
    push rax
    mov rax, 67
    push rax
    mov rax, 68
    push rax
    ;; /array location
    mov rax, 90
    push rax
    mov rax, 1
    push rax
    mov rcx, rsp
    add rcx, 40
    pop rbx
    shl rbx, 3
    sub rcx, rbx
    pop rax
    mov [rcx], rax
    ;; /print
    ;; /array location
    mov rax, 1
    push rax
    mov rax, rsp
    add rax, 32
    pop rbx
    shl rbx, 3
    sub rax, rbx
    push QWORD [rax] ;; index value
    pop rax
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
    ;; /set
    mov rax, 0
    push rax
    ;; /while
label0:
    mov rax, 26
    push rax
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
    ;; /print
    push QWORD [rsp + 0] ; Variable value
    mov rax, 65
    pop rbx
    add rax, rbx
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 32
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
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
    ;; /set
    mov rax, 26
    push rax
    ;; /print
    mov rax, 10
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    ;; /if
    push QWORD [rsp + 0] ; Variable value
    push QWORD [rsp + 16] ; Variable value
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
    mov rax, 65
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 66
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 67
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
    mov rax, 33
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
    ;; /exit
    mov rax, 0
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    add rsp, 0
    jmp label3
label2:
    ;; /else
    ;; /exit
    mov rax, 1
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    add rsp, 0
label3:
    ;; /if
    mov rax, 60  ; Syscall number 60 = exit
    xor rdi, rdi ; End program with 0
    syscall