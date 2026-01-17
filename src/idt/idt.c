#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "task/task.h"

struct idt_desc idt_descriptors[RASOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

static ISR80H_COMMAND isr80h_commands[RASOS_MAX_ISR80H_COMMANDS];
extern void idt_load(struct idtr_desc* ptr);
extern void int21h();
extern void no_interrupt();
extern void isr80h_wrapper();

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
    idt_set(0x80, isr80h_wrapper);

    // Load the interrupt descriptor table.
    // This is actually defined in idt.asm. Address is resolved during linking.
    idt_load(&idtr_descriptor);
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command) {
    if (command_id < 0 || command_id >= RASOS_MAX_ISR80H_COMMANDS) {
        panic("Invalid command_id in registeration\n");
    }

    if (isr80h_commands[command_id]) {
        panic("Command already taking in registeration\n");
    }

    isr80h_commands[command_id] = command;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame) {
    void* result = 0;

    if(command < 0 || command >= RASOS_MAX_ISR80H_COMMANDS) {
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func) {
        return 0;
    }

    result = command_func(frame);
    return result;
}

void* isr80h_handler(int command, struct interrupt_frame* frame) {
    void* res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}
