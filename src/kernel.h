#ifndef KERNEL_H
#define KERNEL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 20
#define VIDEO_MEM_ADDR 0xB8000

#define RASOS_PATH_MAX_LEN 128

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

void kernel_main();
void terminal_print(const char* str);
void panic(const char* msg);

#endif
