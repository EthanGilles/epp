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
    ;; /init list
    mov rax, 51
    push rax
    ;; /init list
    mov rax, 50
    push rax
    mov rax, 48
    push rax
    ;; /init list
    mov rax, 65
    push rax
    mov rax, 66
    push rax
    mov rax, 67
    push rax
    ;; /print
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
    ;; /print
    ;; /array location
    mov rax, 0
    push rax
    mov rax, rsp
    add rax, 40
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
    ;; /print
    ;; /array location
    mov rax, 1
    push rax
    mov rax, rsp
    add rax, 24
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
    ;; /exit
    mov rax, 0
    push rax
    mov rax, 60  ; Syscall number 60 = exit
    pop rdi
    syscall
    ;; /end

    mov rax, 60  ; Syscall number 60 = exit
    xor rdi, rdi ; End program with 0
    syscall