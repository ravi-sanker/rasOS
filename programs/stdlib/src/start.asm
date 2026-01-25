[BITS 32]

global _start
extern main

section .asm

; In the linker file of user programs, we define _start as the entry point.
; This is also the convention in real life C programs. 
_start:
    call main
    ret
