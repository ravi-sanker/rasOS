#include "task.h"
#include "config.h"
#include "kernel.h"
#include "status.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"
#include "memory/paging/paging.h"


struct task* current_task = 0;
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process) {
    memset(task, 0, sizeof(struct task));

    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_BY_ALL);
    if (!task->page_directory) {
        return -EIO;
    }

    task->registers.ip = RASOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = RASOS_PROGRAM_VIRTUAL_STACK_ADDRESS_BOTTOM;

    task->process = process;

    return 0;
}

struct task* task_current() {
    return current_task;
}

struct task* task_new(struct process* process) {
    int res = 0;
    struct task* task = kzalloc(sizeof(struct task));
    if (!task) {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != OK) {
        goto out;
    }

    if (task_head == 0) {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:    
    if (ISERR(res)) {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

struct task* task_get_next() {
    if (!current_task->next) {
        return task_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task* task) {
    if (task->prev) {
        task->prev->next = task->next;
    } else if (task == task_head) { // if condition not really required, but anyway
        task_head = task->next;
    }

    if (task == current_task) {
        current_task = task_get_next();
    }
}

int task_free(struct task* task) {
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    // Finally free the task data
    kfree(task);
    return 0;
}

int task_switch(struct task* task) {
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

int task_page() {
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_page_task(struct task* task) {
    user_registers();
    paging_switch(task->page_directory);
    return 0;
}

void task_run_first_ever_task() {
    if (!current_task) {
        panic("task_run_first_ever_task(): No current task exists!\n");
    }

    task_switch(task_head);
    task_return(&task_head->registers);
}

void task_save_state(struct task *task, struct interrupt_frame *frame) {
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame *frame) {
    if (!task_current()) {
        panic("No current task to save\n");
    }

    struct task *task = task_current();
    task_save_state(task, frame);
}

// copy_string_from task copies the bytes pointed to by virtual_addr
// of the task to the phys_addr.
//
// Why can't we just disable paging and read from the physical address directly?
// I understand the virtual address could be mapped to something else in the kernel's
// page tables, but we can simply disable paging and enable.
int copy_string_from_task(struct task* task, void* virtual_addr, void* phys_addr, int max) {
    if (max >= PAGE_SIZE) {
        return -EINVARG;
    }

    int res = 0;
    char* string_cpy = kzalloc(max);
    if (!string_cpy) {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->directory_entry;

    // Preserver the current value as we will be overwriting it.
    uint32_t page_table_entry = paging_get_table_entry(task_directory, string_cpy);
    paging_map(task->page_directory, string_cpy, string_cpy, PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_BY_ALL);
    paging_switch(task->page_directory);
    strncpy(string_cpy, virtual_addr, max);
    kernel_page();

    // Restore the overwritten page table entry for string_cpy in the task's page table.
    res = paging_set(task_directory, string_cpy, page_table_entry);
    if (res < 0) {
        res = -EIO;
        goto out_free;
    }

    strncpy(phys_addr, string_cpy, max);

out_free:
    kfree(string_cpy);
out:
    return res;
}


void* task_get_stack_item(struct task* task, int index) {
    void* result = 0;
    uint32_t* sp_ptr = (uint32_t*)(task->registers.esp);

    task_page_task(task);
    result = (void*)sp_ptr[index];
    kernel_page();

    return result;
}
