#include "gdt.h"
#include "kernel.h"

void encode_gdt_entry(uint8_t* out, struct gdt_structured in) {
    // Limit is stored in a weird way in the gdt entry.
    // limit 0-15 corresponds to the 0th byte, limit 16-19 corresponds to the lower 6th byte.
    // If G in flags (first bit of upper 6th byte) is set, we are talking abt 4KiB blocks.
    // We are choosing 64KiB as the limit to switch over to page sized blocks.
    if ((in.limit > 65536) && ((in.limit & 0xFFF) != 0xFFF)) {
        panic("encode_gdt_entry: Invalid Argument\n");
    }

    out[6] = 0b01000000;
    if (in.limit > 65536) {
        in.limit = in.limit >> 12;
        // Set the granularity flag.
        out[6] = 0b11000000;
    }

    out[0] = in.limit & 0xFF;
    out[1] = (in.limit >> 8) & 0xFF;
    out[6] |= (in.limit >> 16) & 0x0F;

    out[2] = in.base & 0xFF;
    out[3] = (in.base >> 8) & 0xFF;
    out[4] = (in.base >> 16) & 0xFF;
    out[7] = (in.base >> 24) & 0xFF;

    out[5] = in.access_byte;
}

void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries) {
    for(int i = 0; i < total_entries; i++) {
        encode_gdt_entry((uint8_t*)&gdt[i], structured_gdt[i]);
    }
}