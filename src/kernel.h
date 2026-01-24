#ifndef KERNEL_H
#define KERNEL_H

#define RASOS_PATH_MAX_LEN 128

#define ERROR(value) (void*)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

void kernel_main();
void terminal_print(const char* str);
void panic(const char* msg);
void kernel_page();
void terminal_default_putchar(char c, char colour);

#endif
