; kernel.asm

BITS 32

global _start

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
    
    jmp $