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

char tolower(char ch) {
    if (ch >= 'A' && ch <= 'Z') {
        ch += 32;
    }

    return ch;
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

int strncmp(const char* str1, const char* str2, int n) {
    unsigned char u1, u2;

    while(n-- > 0) {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

int strnlen_terminator(const char* str, int max, char terminator) {
    int i = 0;
    for(i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }

    return i;
}

int istrncmp(const char* s1, const char* s2, int n) {
    unsigned char u1, u2;
    while(n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

char* strncpy(char* dest, const char* src, int count) {
    int i = 0;
    for (i = 0; i < count-1; i++) {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}
