; boot.asm
; Description: A bootloader that simply prints Hello World!
 

ORG 0   
BITS 16 ;tell the assembler this is a 16-bit code

_start:             ;this is to account 
    jmp short start ;for the 
    nop             ;BIOS Parameter Block
times 33 db 0       ;(BPB)


start:
    jmp 0x7C0:main

main:
    cli
    mov ax, 0x7C0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0Eh
    int 0x10
    ret

message:
    db  'Hello World!', 0

times 510 - ($ - $$) db 0   ;zerofill upto 510 bytes
dw 0xAA55                   ;boot sector signature