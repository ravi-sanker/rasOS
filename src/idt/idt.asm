; idt.asm

section .asm

extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
extern interrupt_handler

; Mark idt_load as global so that we can call it in C.
global idt_load
global no_interrupt
global enable_interrupts
global disable_interrupts
global isr80h_wrapper
global interrupt_pointer_table

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


%macro interrupt 1
    global int%1
    int%1:
        pushad
        push esp        ; 2nd argument to c handler (pointer to all the above registers)
        push dword %1   ; 1st argument to c handler, expected to contain sycall number
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

; Define all the interrupts from 0 to 511.
%assign i 0
%rep 512
    interrupt i
%assign i i+1
%endrep

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

%macro interrupt_array_entry 1
    dd int%1
%endmacro

interrupt_pointer_table:
%assign i 0
%rep 512
    interrupt_array_entry i
%assign i i+1
%endrep



