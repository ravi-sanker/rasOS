#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "task/task.h"
#include "status.h"

struct idt_desc idt_descriptors[RASOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;
static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[RASOS_TOTAL_INTERRUPTS];
static ISR80H_COMMAND isr80h_commands[RASOS_MAX_ISR80H_COMMANDS];

extern void idt_load(struct idtr_desc* ptr);
extern void isr80h_wrapper();
extern void* interrupt_pointer_table[RASOS_TOTAL_INTERRUPTS];

#define MASTER_PIC_COMMAND_AND_STATUS_REG   0x20
#define END_OF_INTERRUPT                    0x20

void interrupt_handler(int interrupt, struct interrupt_frame* frame) {
    kernel_page();
    if (interrupt_callbacks[interrupt] != 0) {
        task_current_save_state(frame);
        interrupt_callbacks[interrupt]();
    }

    task_page();
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
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for(int i = 0; i < RASOS_TOTAL_INTERRUPTS; i++) {
        idt_set(i, interrupt_pointer_table[i]);
    }
    idt_set(0, idt_zero);
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

int idt_register_interrupt_callback(int interrupt, INTERRUPT_CALLBACK_FUNCTION interrupt_callback) {
    if (interrupt < 0 || interrupt >= RASOS_TOTAL_INTERRUPTS) {
        return -EINVARG;
    }

    interrupt_callbacks[interrupt] = interrupt_callback;
    return 0;
}
