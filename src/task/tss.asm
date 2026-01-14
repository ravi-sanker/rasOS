section .asm

global tss_load

; Refer to https://wiki.osdev.org/Task_State_Segment.
tss_load:
    push ebp
    mov ebp, esp
    mov ax, [ebp+8] ; TSS Segment
    ltr ax
    pop ebp
    ret