#include "keyboard.h"
#include "config.h"
#include "status.h"
#include "kernel.h"
#include "task/process.h"
#include "task/task.h"
#include "keyboard/qwerty.h"

static struct keyboard* keyboard_list_head = 0;
static struct keyboard* keyboard_list_last = 0;

int keyboard_insert(struct keyboard* keyboard) {
    int res = 0;
    if (keyboard->init == 0) {
        res = -EINVARG;
        goto out;
    }

    if (keyboard_list_last) {
        keyboard_list_last->next = keyboard;
        keyboard_list_last = keyboard;
    } else {
        keyboard_list_head = keyboard;
        keyboard_list_last = keyboard;
    }
    
    res = keyboard->init();
out:
    return res;
}

void keyboard_init() {
    keyboard_insert(qwerty_init());
}

void keyboard_backspace(struct process* process) {
    process->keyboard.tail -= 1;
    if (process->keyboard.tail < 0) {
        process->keyboard.tail = 0;
    }
    // Note that tail is where the next character should go.
    // Just clean it up.
    process->keyboard.buffer[process->keyboard.tail] = 0;
}

void keyboard_push(char c) {
    // Use the foreground process.
    struct process* process = process_current();
    if (!process) {
        return;
    }

    if (c == 0) {
        return;
    }

    process->keyboard.buffer[process->keyboard.tail] = c;
    process->keyboard.tail++;
    if (process->keyboard.tail >= RASOS_KEYBOARD_BUFFER_SIZE) {
        process->keyboard.tail = 0;
    }
}

// keyboard_pop returns the char stored at the head and increments the head pointer.
char keyboard_pop() {
    if (!task_current()) {
        return 0;
    }

    struct process* process = task_current()->process;
    char ch = process->keyboard.buffer[process->keyboard.head];
    process->keyboard.buffer[process->keyboard.head] = 0;
    process->keyboard.head++;
    if (process->keyboard.head >= RASOS_KEYBOARD_BUFFER_SIZE) {
        process->keyboard.head = 0;
    }
    return ch;
}
