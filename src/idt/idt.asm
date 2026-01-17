; idt.asm

section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler

; Mark idt_load as global so that we can call it in C.
global idt_load
global int21h
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper

enable_interrupts:
    sti
    ret

disable_interrupts:
    cli
    ret

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

isr80h_wrapper:
    cli
    pushad
    push esp ; 2nd argument to c handler (pointer to all the above registers)
    push eax ; 1st argument to c handler, expected to contain sycall number
    call isr80h_handler
    mov dword[tmp_res], eax ; eax expected to have return value, we are doing this for readability
    add esp, 8

    popad
    mov eax, [tmp_res]
    iretd

section .data
tmp_res: dd 0



