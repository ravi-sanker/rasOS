#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct interrupt_frame;
typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);

// Refer https://wiki.osdev.org/Interrupt_Descriptor_Table.
struct idt_desc {
    uint16_t offset_1;        // offset bits 0..15
    uint16_t selector;        // a code segment selector in GDT or LDT
    uint8_t  zero;            // unused, set to 0
    uint8_t  type_attributes; // gate type, dpl, and p fields
    uint16_t offset_2;        // offset bits 16..31
} __attribute__((packed));

struct idtr_desc {
    uint16_t limit; // size of descriptor table - 1
    uint32_t base;  // base address of the start of the IDT
} __attribute__((packed));

struct interrupt_frame {
    // These are pushed due to pushad.
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    // These are pushed due to interrupt.
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

void enable_interrupts();
void disable_interrupts();
void idt_init();
void isr80h_register_command(int command_id, ISR80H_COMMAND command);

#endif
