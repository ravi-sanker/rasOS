; boot.asm

ORG 0x7c00  
; When this code is run, the CPU will be in real mode. So we need to tell the
; assembler to use 16-bit code.
BITS 16

CODE_SEG_GDT_OFFSET equ gdt_code - gdt_start
DATA_SEG_GDT_OFFSET equ gdt_data - gdt_start

; The BIOS might use the first 33 bytes to write data known as the
; BIOS Parameter Block (BPB). Refer wiki.osdev.org/FAT.
_start:            
    jmp short start ; The first two bytes need be this for some BIOS to work.
    nop             
times 33 db 0       
 
start:
    jmp 0:main ; Set the code segment to 0x7c0.

main:
    cli ; Disable interrupts.

    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00  ; This is done to not conflict the program binary with the stack.

    sti ; Enable interrupts.

; Refer https://wiki.osdev.org/Global_Descriptor_Table.
.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    ; In protected mode, segmentation works differently. Here, the "segment part"
    ; actually refers to the offset into the GDT. That's the reason we have 
    ; labels to calculate the offset. Refer https://wiki.osdev.org/Segmentation. 
    jmp CODE_SEG_GDT_OFFSET:load32 

; GDT
gdt_start:
; Each entry is 64 bits / 8 bytes.
; The first entry needs to be null.
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code:       ; CS SHOULD POINT TO THIS
    dw 0xffff   ; Segment limit first 0-15 bits
    dw 0        ; Base first 0-15 bits
    db 0        ; Base 16-23 bits
    db 0x9a     ; Access byte
    db 11001111b; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

; offset 0x10
gdt_data:       ; DS, SS, ES, FS, GS
    dw 0xffff   ; Segment limit first 0-15 bits
    dw 0        ; Base first 0-15 bits
    db 0        ; Base 16-23 bits
    db 0x92     ; Access byte
    db 11001111b; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; -1 is due to GDT peculiarities.
    dd gdt_start

; Now we are in protected mode.
; This is 32 bit code.
BITS 32

load32:
    mov ax, DATA_SEG_GDT_OFFSET
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp
    jmp $

; BIOS checks bootable devices for the boot signature, which is 0x55AA present at
; the 510th and 511th byte of the 0th sector of the device. We are writing a
; bootloader and we need to include the signature at the correct bytes.
; So, first we need to fill up till 510th byte (exclusive). Next, we will have to
; write the boot signature. 
times 510 - ($ - $$) db 0
dw 0xAA55 ; It's 0xAA55 due to the little endian format.