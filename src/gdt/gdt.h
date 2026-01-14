#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Refer https://wiki.osdev.org/Global_Descriptor_Table.
struct gdt {
    uint16_t limit_0_15;
    uint16_t base_0_15;
    uint8_t base_16_23;
    uint8_t access_byte;
    uint8_t high_flags; // the low 4 bits are for the limit, but we are not using them.
    uint8_t base_24_31;
} __attribute__((packed));

struct gdt_structured {
    uint32_t base;
    uint32_t limit;
    uint8_t access_byte;
};

void gdt_load(struct gdt* gdt, int size);
void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries);

#endif