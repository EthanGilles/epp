section .data
    char db 0    ;; Allocate byte for a char

section .text
    global _start

_start:
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
    ;; push-pop removed rax
    pop rbx
    add rax, rbx
    ;; push-pop removed rax
    mov [char], al  ;; Store rax in char
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    mov rsi, char
    syscall
    mov rax, 32
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
    ;; /reset
    mov rax, 1
    push rax
    push QWORD [rsp + 8] ; Variable value
    pop rax
    pop rbx
    add rax, rbx
    ;; push-pop removed rax
    mov [rsp + 0], rax
    add rsp, 0
    jmp label0
label1:
    ;; /while
    ;; /if
    mov rax, 26
    push rax
    push QWORD [rsp + 8] ; Variable value
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