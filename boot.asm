; boot.asm
; Description: A bootloader that simply prints Hello World!
 

ORG 0  
; When this code is run, the CPU will be in real mode. So we need to tell the
; assembler to use 16-bit code.
BITS 16

; The BIOS might use the first 33 bytes to write data known as the
; BIOS Parameter Block (BPB). Refer wiki.osdev.org/FAT.
_start:            
    jmp short start ; The first two bytes need be this for some BIOS to work.
    nop             
times 33 db 0       
 
start:
    jmp 0x7c0:main ; Set the code segment to 0x7c0.

main:
    cli ; Disable interrupts.

    ; The BIOS will load this program at 0x7c00.
    ; So we need the assembler to calculate the addresses properly. So we will
    ; load 0x7C0 (it'll get multiplied by 16) to the data and extra segment registers.
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00    ; Just load 0 into the stack segment.
    mov ss, ax
    mov sp, 0x7c00  ; This is done to not conflict the program binary with the stack.

    sti ; Enable interrupts.

    mov si, message
    call print
    jmp $

print:
    mov bx, 0
.loop:
    lodsb ; Load the byte pointed by ds:si to al and increment si.
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh ; If ah is this, calling interrupt 10 means print whatever is
    int 0x10    ; is in the al register.
    ret

message:
    db  'Hello World!', 0

; BIOS checks bootable devices for the boot signature, which is 0x55AA present at
; the 510th and 511th byte of the 0th sector of the device. We are writing a
; bootloader and we need to include the signature at the correct bytes.
; So, first we need to fill up till 510th byte (exclusive). Next, we will have to
; write the boot signature. 
times 510 - ($ - $$) db 0
dw 0xAA55 ; It's 0xAA55 due to the little endian format.