; idt.asm

section .asm

; Mark idt_load as global so that we can call it in C.
global idt_load

; When this is called in C, the address of the interrupt descriptor table register
; should be passed as the first argument.
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; points to first argument passed to the function
    lidt [ebx]          ; load idt with address of first argument

    pop ebp
    ret