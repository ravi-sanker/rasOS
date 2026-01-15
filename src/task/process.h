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
};

#endif