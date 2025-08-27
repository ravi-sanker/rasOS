; io.asm

section .asm

; Declare these as global so that we can call it in C.
global insb
global insw
global outb
global outw

; Reference: https://c9x.me/x86/html/file_module_x86_id_139.html
insb:
    push ebp
    mov ebp, esp

    xor eax, eax

    ; [EBP] is the saved content of EBP register.
    ; [EBP + 4] is the return address.
    ; [EBP + 8] is the first argument, which is expected to be the port number.
    mov dx, [ebp + 8]
    in al, dx ; input byte from i/o port in dx into al

    pop ebp
    ret

insw:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov dx, [ebp + 8]
    in ax, dx; input word from i/o port in dx into ax

    pop ebp
    ret

; Reference: https://c9x.me/x86/html/file_module_x86_id_222.html
outb:
    push ebp
    mov ebp, esp

    ; [EBP] + 8, the first argument is the port number.
    ; [EBP] + 12, the second argument is the data to be output.
    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al ; output byte in al to i/o port in dx

    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp

    ; [EBP] + 8, the first argument is the port number.
    ; [EBP] + 12, the second argument is the data to be output.
    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, ax ; output word in ax to i/o port in dx

    pop ebp
    ret