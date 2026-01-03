#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define VIDEO_MEM_ADDR 0xB8000

#define RASOS_PATH_MAX_LEN 128

void kernel_main();
void terminal_print(const char* str);

#endif
