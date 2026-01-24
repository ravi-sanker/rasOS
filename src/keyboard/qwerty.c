#include "qwerty.h"
#include "keyboard.h"
#include "io/io.h"
#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "idt/idt.h"
#include "task/task.h"

int qwerty_keyboard_init();

// Refer https://wiki.osdev.org/PS/2_Keyboard.
static uint8_t keyboard_scan_set_one[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

struct keyboard qwerty_keyboard = {
    .name = {"QWERTY"},
    .init = qwerty_keyboard_init
};

void qwerty_keyboard_handle_interrupt();

int qwerty_keyboard_init() {
    // Refer https://wiki.osdev.org/I8042_PS/2_Controller.
    idt_register_interrupt_callback(ISR_KEYBOARD_INTERRUPT, qwerty_keyboard_handle_interrupt);
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

uint8_t qwerty_keyboard_scancode_to_char(uint8_t scancode) {
    size_t size_of_keyboard_set_one = sizeof(keyboard_scan_set_one) / sizeof(uint8_t);
    if (scancode > size_of_keyboard_set_one) {
        return 0;
    }

    char c = keyboard_scan_set_one[scancode];
    return c;
}


void qwerty_keyboard_handle_interrupt(){
    kernel_page();
    uint8_t scancode = 0;
    scancode = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT);

    if(scancode & QWERTY_KEYBOARD_KEY_RELEASED) {
        return;
    }

    uint8_t c = qwerty_keyboard_scancode_to_char(scancode);
    if (c != 0) {
        keyboard_push(c);
    }

    task_page();
}

struct keyboard* qwerty_init() {
    return &qwerty_keyboard;
}
