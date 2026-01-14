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

uint16_t *video_mem = 0;
uint16_t terminal_current_row = 0;
uint16_t terminal_current_col = 0;
static struct paging_4gb_chunk* kernel_chunk = 0;

uint16_t terminal_make_char(char c, char colour) {
    // colour needs to come to the left due to the little endian format.
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_default_putchar(char c, char colour) {
    if (c == '\n') {
        terminal_current_row += 1;
        terminal_current_col = 0;
        return;
    }

    terminal_putchar(terminal_current_col, terminal_current_row, c, colour);
    terminal_current_col += 1;
    if (terminal_current_col >= VGA_WIDTH) {
        terminal_current_col = 0;
        terminal_current_row += 1;
    }
}

void terminal_print(const char* str) {
    size_t len = strlen(str);
    for(int i = 0; i < len; i++) {
        terminal_default_putchar(str[i], 15);
    }
}

void terminal_initialize() {
    video_mem = (uint16_t*)(VIDEO_MEM_ADDR);
    for(int y = 0; y < VGA_HEIGHT; y++) {
        for(int x = 0; x < VGA_WIDTH; x++) {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

void panic(const char* msg) {
    terminal_print(msg);
    while(1) {}
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
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));


    enable_paging();

    enable_interrupts();

    int fd = fopen("0:/hello.txt", "r");
    if (fd) {
        terminal_print("We opened hello.txt\n");
        char buf[20];
        fseek(fd, 2, SEEK_SET);
        fread(buf, 20, 1, fd);
        terminal_print(buf);

        struct file_stat stat;
        fstat(fd, &stat);

        fclose(fd);
        terminal_print("end");
    }
    while(1) {}
}
