#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "task.h"
#include "config.h"
#include "kernel.h"

struct process {
    uint16_t id;
    struct task* task;

    // This is basically the executable.
    char filename[RASOS_PATH_MAX_LEN];

    // The memory allocations (malloc) of the process.
    // Just to keep track so that we can free it when the process is killed.
    void* allocations[RASOS_MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the executable in memory.
    void* ptr;

    // The size of the data pointed to by "ptr".
    uint32_t size;

    // The physical pointer to the stack memory.
    void* stack;

    struct keyboard_buffer {
        char buffer[RASOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    } keyboard;
};

int process_load(const char* filename, struct process** process);
struct process* process_current();
struct process* process_get(int process_id);
int process_switch(struct process* process);
int process_load_switch(const char* filename, struct process** process);

#endif