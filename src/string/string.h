#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdbool.h>

size_t strlen(const char* str);
bool is_digit(char ch);
char* strcpy(char* dest, const char* src);

#endif