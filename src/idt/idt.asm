; idt.asm

section .asm

extern int21h_handler
extern no_interrupt_handler

; Mark idt_load as global so that we can call it in C.
global idt_load
global int21h
global no_interrupt

; When this is called in C, the address of the interrupt descriptor table register
; should be passed as the first argument.
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]  ; points to first argument passed to the function
    lidt [ebx]          ; load idt with address of first argument

    pop ebp
    ret

int21h:
    cli
    pushad

    call int21h_handler

    popad
    sti
    iret

no_interrupt:
    cli
    pushad

    call no_interrupt_handler

    popad
    sti
    iret
