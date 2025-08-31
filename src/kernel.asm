; kernel.asm

BITS 32

global _start
global MASTER_PIC_COMMAND_AND_STATUS_REG

; kernel_main is exposed in kernel.h and implemented in kernel.c.
; We are using extern here to tell the assembler that this symbol needs
; to be resolved in the linking stage.
extern kernel_main

DATA_SEG_GDT_OFFSET equ 0x10 ; hardcode it for now

MASTER_PIC_COMMAND_AND_STATUS_REG   equ 0x20
MASTER_PIC_IMR_AND_DATA_REG         equ 0x21
SLAVE_PIC_COMMAND_AND_STATUS_REG    equ 0xA0
SLAVE_PIC_IMR_AND_DATA_REG          equ 0xA1

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

    ; Program the Programmable Interrupt Controller (PIC) or 8259A.
    ; Reference: http://www.brokenthorn.com/Resources/OSDevPic.html

    ; ICW1
    mov al, 00010001b   ; set the initialization bit and IC4.
    out MASTER_PIC_COMMAND_AND_STATUS_REG, al
    out SLAVE_PIC_COMMAND_AND_STATUS_REG, al

    ; ICW2
    mov al, 0x20     ; hardware interrupts will start from 32 as 0-31 is taken up
    out MASTER_PIC_IMR_AND_DATA_REG, al
    mov al, 0x28     ; slave PIC will start from 40
    out SLAVE_PIC_IMR_AND_DATA_REG, al

    ; ICW3
	mov	al, 0x4		; 0x04 => 0100, second bit (IR line 2)
	out	MASTER_PIC_IMR_AND_DATA_REG, al	; write to data register of primary PIC
	mov	al, 0x2		; 010=> IR line 2
	out	SLAVE_PIC_IMR_AND_DATA_REG, al	; write to data register of secondary PIC

    ; ICW4
    mov	al, 1		; bit 0 enables 80x86 mode
	out	MASTER_PIC_IMR_AND_DATA_REG, al
	out	SLAVE_PIC_IMR_AND_DATA_REG, al

    call kernel_main

    jmp $

; This is done so that the kernel code takes up an entire sector and the C code
; that follows this won't be misaligned. Note that kernel.asm needs to come at
; the very beginning and isn't part of the asm section.
times 512 - ($ - $$) db 0
