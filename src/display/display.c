#include "display.h"
#include <stdint.h>
#include "string/string.h"

uint16_t *video_mem = 0;
uint16_t terminal_current_row = 0;
uint16_t terminal_current_col = 0;

uint16_t terminal_make_char(char c, char colour) {
    // colour needs to come to the left due to the little endian format.
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour) {
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_backspace() {
    if (terminal_current_row == 0 && terminal_current_col == 0) {
        return;
    }

    if (terminal_current_col == 0){
        terminal_current_row -= 1;
        terminal_current_col = VGA_WIDTH;
    }

    terminal_current_col--;
    terminal_default_putchar(' ', 15);
    terminal_current_col--;
}

void terminal_default_putchar(char c, char colour) {
    if (c == '\n') {
        terminal_current_row += 1;
        terminal_current_col = 0;
        return;
    }

    if (c == 0x08) {
        terminal_backspace();
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
