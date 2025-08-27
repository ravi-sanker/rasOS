#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_ENTRY_FREE   0b00000000
#define HEAP_BLOCK_ENTRY_TAKEN  0b00000001
#define HEAP_BLOCK_HAS_NEXT     0b10000000
#define HEAP_BLOCK_IS_FIRST     0b01000000

// Each table entry is just 1 byte.
// 0b76543210
// 7 -> Is the next block also part of the current allocation?
// 6 -> Is this the first block of an allocation?
// 5, 4, 3, 2, 1 -> unused.
// 0 -> Is the current entry taken?
typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

// The heap table will contain a pointer to the start of the entries and the
// number of such entries.
struct heap_table {
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
};

// The heap itself will contain a pointer to the table and starting address of
// the heap.
struct heap {
    struct heap_table* table;
    void* start_address;
};

int heap_create(struct heap* heap, void* table_start_addr, void* heap_start_addr, size_t heap_size);
void* heap_malloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* ptr);

#endif
