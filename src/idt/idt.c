#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"

struct idt_desc idt_descriptors[RASOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();

#define MASTER_PIC_COMMAND_AND_STATUS_REG   0x20
#define END_OF_INTERRUPT                    0x20

void int21h_handler() {
    terminal_print("Keyboard pressed!");
    outb(MASTER_PIC_COMMAND_AND_STATUS_REG, END_OF_INTERRUPT);
}

void no_interrupt_handler() {
    outb(MASTER_PIC_COMMAND_AND_STATUS_REG, END_OF_INTERRUPT);
}

void idt_zero() {
    terminal_print("Divide by zero error\n");
}

void idt_set(int interrupt_number, void* address) {
    struct idt_desc* desc = &idt_descriptors[interrupt_number];
    desc->offset_1 = (uint32_t)address & 0x0000FFFF;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    // Set present bit to 1, DPL to 3 and gate type to 0xE.
    desc->type_attributes = 0xEE;
    desc->offset_2 = (uint32_t)address >> 16;
}

void idt_init() {
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t) idt_descriptors;

    for(int i = 0; i < RASOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, no_interrupt);
    }
    idt_set(0, idt_zero);
    idt_set(0x21, int21h);

    // Load the interrupt descriptor table.
    // This is actually defined in idt.asm. Address is resolved during linking.
    idt_load(&idtr_descriptor);
}