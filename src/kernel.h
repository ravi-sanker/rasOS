#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define VIDEO_MEM_ADDR 0xB8000

void kernel_main();
void terminal_print(const char* str);

#endif