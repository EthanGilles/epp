section .data
    char db 0    ;; Allocate byte for a char

section .text
    global _start

_start:
    ;; /set
    mov rax, 2
    push rax
    mov rax, 3
    push rax
    mov rax, 2
    ;; push-pop removed rax
    pop rbx
    mul rbx
    push rax
    mov rax, 10
    ;; push-pop removed rax
    pop rbx
    sub rax, rbx
    ;; push-pop removed rax
    pop rbx
    div rbx
    push rax
    ;; /set
    mov rax, 1
    push rax
    ;; /set
    mov rax, 0
    push rax
    ;; /if
    push QWORD [rsp + 0] ; Variable value
    pop rax
    test rax, rax
    jz label0
    ;; /reset
    mov rax, 2
    push rax
    mov rax, 15
    ;; push-pop removed rax
    pop rbx
    add rax, rbx
    ;; push-pop removed rax
    mov [rsp + 0], rax
    add rsp, 0
    jmp label1
label0:
    ;; /else
    ;; /reset
    mov rax, 20
    ;; push-pop removed rax
    mov [rsp + 16], rax
    add rsp, 0
label1:
    ;; /if
    ;; /print
    mov rax, 65
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 66
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 67
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 10
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    ;; /print
    mov rax, 65
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 66
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 67
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 10
    ;; push-pop removed rax
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
    mov rax, 60  ; Syscall number 60 = exit
    xor rdi, rdi ; End program with 0
    syscall