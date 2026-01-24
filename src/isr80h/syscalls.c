#include "syscalls.h"
#include "idt/idt.h"
#include "task/task.h"
#include "kernel.h"
#include "keyboard/keyboard.h"

void* isr80h_command0_sum(struct interrupt_frame* frame) {
    int v2 = (int)task_get_stack_item(task_current(), 1);
    int v1 = (int)task_get_stack_item(task_current(), 0);
    return (void*)(v1 + v2);
}

void* isr80h_command1_print(struct interrupt_frame* frame) {
    void* user_space_msg_buffer = task_get_stack_item(task_current(), 0);
    char buf[1024];
    copy_string_from_task(task_current(), user_space_msg_buffer, buf, sizeof(buf));

    terminal_print(buf);
    return 0;
}

void* isr80h_command2_getkey(struct interrupt_frame* frame) {
    char c = keyboard_pop();
    return (void*)((int)c);
}

void* isr80h_command3_putchar(struct interrupt_frame* frame) {
    char c = (char)(int) task_get_stack_item(task_current(), 0);
    terminal_default_putchar(c, 15);
    return 0;
}
