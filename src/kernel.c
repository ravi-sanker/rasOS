#include "kernel.h"
#include <stdint.h>
#include <stddef.h>
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "fs/file.h"
#include "gdt/gdt.h"
#include "config.h"
#include "memory/memory.h"
#include "task/tss.h"
#include "status.h"
#include "task/process.h"
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

static struct paging_4gb_chunk* kernel_chunk = 0;

extern void kernel_registers();

void panic(const char* msg) {
    terminal_print(msg);
    while(1) {}
}

void kernel_page() {
    kernel_registers();
    paging_switch(kernel_chunk);
}

struct tss tss;
struct gdt gdt_real[RASOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[RASOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .access_byte = 0x00},                 // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .access_byte = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .access_byte = 0x92},           // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .access_byte = 0xf8},           // User code segment
    {.base = 0x00, .limit = 0xffffffff, .access_byte = 0xf2},           // User data segment
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .access_byte = 0xE9}   // TSS Segment
};

void kernel_main() {
    terminal_initialize();
    terminal_print("Hello, World!\nWelcome to RasOS!\n");

    memset(gdt_real, 0x00, sizeof(gdt_real));
    gdt_structured_to_gdt(gdt_real, gdt_structured, RASOS_TOTAL_GDT_SEGMENTS);

    // Load the gdt
    gdt_load(gdt_real, sizeof(gdt_real));

    // Initialize the heap.
    kheap_init();

    // Initialize the file systems.
    fs_init();

    // Search and initialize the disk;
    disk_search_and_init();

    // Initialize the interrupt descriptor table.
    idt_init();

    // Setup the TSS
    memset(&tss, 0x00, sizeof(tss));
    tss.esp0 = 0x600000;
    tss.ss0 = KERNEL_DATA_SELECTOR;

    // Load the TSS
    tss_load(0x28); // Remember, this is the offset into the GDT.

    // Setup paging.
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_BY_ALL);
    paging_switch(kernel_chunk);


    enable_paging();

    isr80h_register_commands();

    keyboard_init();

    struct process* process = 0;
    int res = process_load_switch("0:/blank.bin", &process);
    if (res != OK) {
        panic("Failed to load blank.bin\n");
    }

    
    task_run_first_ever_task();
    while(1) {}
}
