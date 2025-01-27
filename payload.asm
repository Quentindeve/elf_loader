[BITS 64]

section .text
global _start
    _start:

        ;; sys_write("Hello World\n", 1);
        mov rax, 1
        mov rdi, 1
        ;; move message address to register
        lea rsi, [msg]
        mov rdx, msg_size
        syscall

        ;; sys_exit(69)
        mov rax, 60
        mov rdi, 69
        syscall

section .data
    msg: db `Hello World\n`, 0, 13
    msg_size: equ $-msg
