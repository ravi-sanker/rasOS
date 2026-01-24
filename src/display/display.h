#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define VGA_WIDTH       80
#define VGA_HEIGHT      20
#define VIDEO_MEM_ADDR  0xB8000

extern uint16_t *video_mem;
extern uint16_t terminal_current_row;
extern uint16_t terminal_current_col;

void terminal_initialize();
void terminal_print(const char* str);
void terminal_default_putchar(char c, char colour);

#endif
