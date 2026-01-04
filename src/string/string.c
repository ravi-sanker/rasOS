#include "string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while(str[len] != '\0') {
        len++;
    }
    return len;
}

bool is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

char* strcpy(char* dest, const char* src) {
    char* res = dest;
    while(*src != 0) {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}