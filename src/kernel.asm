; kernel.asm

BITS 32

global _start

; kernel_main is exposed in kernel.h and implemented in kernel.c.
; We are using extern here to tell the assembler that this symbol needs
; to be resolved in the linking stage.
extern kernel_main

DATA_SEG_GDT_OFFSET equ 0x10 ; hardcode it for now

_start:
    mov ax, DATA_SEG_GDT_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Enable A20 line.
    in al, 0x92
    or al, 2
    out 0x92, al
    
    call kernel_main

    jmp $

; This is done so that the kernel code takes up an entire sector and the C code
; that follows this won't be misaligned. Note that kernel.asm needs to come at
; the very beginning and isn't part of the asm section.
times 512 - ($ - $$) db 0