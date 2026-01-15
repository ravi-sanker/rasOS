[BITS 32]
section .asm

global restore_general_purpose_registers
global task_return
global user_registers

; uint32_t edi; 0
; uint32_t esi; 4
; uint32_t ebp; 8
; uint32_t ebx; 12
; uint32_t edx; 16
; uint32_t ecx; 20
; uint32_t eax; 24
; uint32_t ip;  28
; uint32_t cs;  32
; uint32_t flags; 36
; uint32_t esp;   40
; uint32_t ss;    44

; void task_return(struct registers* regs);
; This is used to move to user land from kernel mode. We are going to do this by
; using the iretd call. This expects the following values to be in the stack:
; ss, esp, flags, cs, eip
; You cannot directly update cs and eip registers. So this "hack" is needed.
task_return:
    mov ebp, esp

    ; ebx holds the address of the registers.
    mov ebx, [ebp+4]
    push dword [ebx+44] ; push ss
    push dword [ebx+40] ; push esp

    ; push flags
    pushf
    pop eax
    or eax, 0x200 ; enable interrupt
    push eax

    push dword [ebx+32] ; push cs
    push dword [ebx+28] ; push ip

    ; Set the value of ss to the other segment registers as well.
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword ebx
    call restore_general_purpose_registers
    add esp, 4

    ; Let's leave kernel land and execute in user land!
    iretd
    
; void restore_general_purpose_registers(struct registers* regs);
restore_general_purpose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

; void user_registers()
user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret