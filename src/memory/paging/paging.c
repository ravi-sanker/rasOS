#include "paging.h"
#include "memory/heap/kheap.h"

static uint32_t* current_directory = 0;

extern void paging_load_directory(uint32_t* directory);

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags) {
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGE_DIRECTORY_TOTAL_ENTRIES);

    int offset = 0;
    for(int i = 0; i < PAGE_DIRECTORY_TOTAL_ENTRIES; i++) {
        uint32_t* table = kzalloc(sizeof(uint32_t) * PAGE_TABLE_TOTAL_ENTRIES);

        for(int j = 0; j < PAGE_TABLE_TOTAL_ENTRIES; j++) {
            table[j] = (offset + j * PAGE_SIZE) | flags;
        }
        directory[i] = (uint32_t)table | flags | PAGING_IS_WRITABLE;
        offset += PAGE_SIZE * PAGE_TABLE_TOTAL_ENTRIES;
    }

    struct paging_4gb_chunk* chunk_4gb = kzalloc(sizeof(struct paging_4gb_chunk));
    chunk_4gb->directory_entry = directory;
    return chunk_4gb;
}

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}
