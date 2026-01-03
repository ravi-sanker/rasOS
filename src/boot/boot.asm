; boot.asm

; BIOS loads the bootloader at 0x7c00.
ORG 0x7c00  
; When this code is run, the CPU will be in real mode. So we need to tell the
; assembler to use 16-bit code.
BITS 16

CODE_SEG_GDT_OFFSET equ gdt_code - gdt_start
DATA_SEG_GDT_OFFSET equ gdt_data - gdt_start

; The BIOS might use the first 33 bytes to write data known as the
; BIOS Parameter Block (BPB). Refer wiki.osdev.org/FAT.
_start:            
    jmp short start ; The first two bytes need be this for some BIOS to work. You're
    nop             ; basically jumping over to the instructions and skipping data.  
times 33 db 0       
 
start:
    jmp 0:main ; Make sure CS is set to 0x0000 as BIOS could've set it to 0x07c0.

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
    ; Set 0th bit of CR0 to 1 to enable protected mode.
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    ; In protected mode, segmentation works differently. Here, the "segment part"
    ; actually refers to the offset into the GDT. That's the reason we have 
    ; labels to calculate the offset. Refer https://wiki.osdev.org/Segmentation. 
    jmp CODE_SEG_GDT_OFFSET:load32

;-------------------------------------------------------------------------------
; Global Descriptor Table (GDT)
;-------------------------------------------------------------------------------

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

;-------------------------------------------------------------------------------

BITS 32
load32:
    mov eax, 1          ; starting sector number of our kernel
    mov ecx, 100        ; number of sectors
    mov edi, 0x0100000  ; address to load our kernel into
    call ata_lba_read
    jmp CODE_SEG_GDT_OFFSET: 0x0100000

;-------------------------------------------------------------------------------
; Assembly to load kernel into memory
;-------------------------------------------------------------------------------

ata_lba_read:
    mov ebx, eax, ; Backup the LBA
    ; Send the highest 8 bits of the lba to hard disk controller
    shr eax, 24
    or eax, 0xE0 ; Select the  master drive
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the lba

    ; Send the total sectors to read
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the backup LBA
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send upper 16 bits of the LBA
    mov dx, 0x1F5
    mov eax, ebx ; Restore the backup LBA
    shr eax, 16
    out dx, al
    ; Finished sending upper 16 bits of the LBA

    mov dx, 0x1f7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory
.next_sector:
    push ecx

; Checking if we need to read
.try_again:
    mov dx, 0x1f7
    in al, dx
    test al, 8
    jz .try_again

; We need to read 256 words at a time
    mov ecx, 256
    mov dx, 0x1F0
    rep insw
    pop ecx
    loop .next_sector
    ; End of reading sectors into memory
    ret

;-------------------------------------------------------------------------------

; BIOS checks bootable devices for the boot signature, which is 0x55AA present at
; the 510th and 511th byte of the 0th sector of the device. We are writing a
; bootloader and we need to include the signature at the correct bytes.
; So, first we need to fill up till 510th byte (exclusive). Next, we will have to
; write the boot signature. 
times 510 - ($ - $$) db 0
dw 0xAA55 ; It's 0xAA55 due to the little endian format.
