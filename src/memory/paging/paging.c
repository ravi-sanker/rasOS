#include "paging.h"
#include "memory/heap/kheap.h"
#include <stdint.h>
#include "status.h"

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

void paging_switch(struct paging_4gb_chunk* directory) {
    paging_load_directory(directory->directory_entry);
    current_directory = directory->directory_entry;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk) {
    return chunk->directory_entry;
}

bool paging_is_aligned(void* address) {
    return ((uint32_t)address % PAGE_SIZE) == 0;
}

// paging_get_indexes takes in a virtual address and tells you the page 
// directory index and the page table entry index. 
int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out) {
    if (!paging_is_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t page_number = (uint32_t)virtual_address / PAGE_SIZE;
    *directory_index_out = (page_number / PAGE_TABLE_TOTAL_ENTRIES);
    *table_index_out = page_number - *directory_index_out*PAGE_TABLE_TOTAL_ENTRIES;
    return 0;
}

// paging_set sets the corresponding page table entry of the virtual_address to
// the val that's passed. The starting index of the page directory is expected.
// The page directory entries are expected to be populated.
int paging_set(uint32_t* directory, void* virtual_address, uint32_t val) {
    if (!paging_is_aligned(virtual_address)) {
        return -EINVARG;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virtual_address, &directory_index, &table_index);
    if (res < 0) {
        return res;
    }

    uint32_t page_directory_entry = directory[directory_index];
    // The page_directory_entry holds the address to the page table entry.
    // The address of the page table entry is always 4KiB aligned. Why?
    // Each page table has 1024 entries (32 bits address split into 10+10+12).
    // Each entry is 4 bytes. So each page table is 4KiB in size.
    uint32_t* page_table_address = (uint32_t*)(page_directory_entry & 0xfffff000);
    page_table_address[table_index] = val;

    return 0;
}

void paging_free_4gb(struct paging_4gb_chunk* chunk) {
    // 4GiB corresponds to one full directory table. This has 1024 entries.
    for (int i = 0; i < 1024; i++) {
        uint32_t entry = chunk->directory_entry[i];
        // The top 20 bits correspond to the address of the page table this entry is pointing to.
        uint32_t* table = (uint32_t*)(entry & 0xFFFFF000);
        kfree(table);
    }

    kfree(chunk->directory_entry);
    kfree(chunk);
}

void* paging_align_address(void* ptr) {
    if ((uint32_t)ptr % PAGE_SIZE == 0) {
        return ptr;
    }

    return (void*)((uint32_t)ptr + PAGE_SIZE - ((uint32_t)ptr % PAGE_SIZE));
}

int paging_map(struct paging_4gb_chunk* directory, void* virt, void* phys, int flags) {
    if (((unsigned int)virt % PAGE_SIZE) || ((unsigned int) phys % PAGE_SIZE)) {
        return -EINVARG;
    }

    return paging_set(directory->directory_entry, virt, (uint32_t) phys | flags);
}

int paging_map_range(struct paging_4gb_chunk* directory, void* virt, void* phys, int count, int flags) {
    int res = 0;
    for (int i = 0; i < count; i++) {
        res = paging_map(directory, virt, phys, flags);
        if (res == 0)
            break;
        virt += PAGE_SIZE;
        phys += PAGE_SIZE;
    }

    return res;
}

int paging_map_to(struct paging_4gb_chunk *directory, void *virt, void *phys, void *phys_end, int flags) {
    int res = 0;
    if ((uint32_t)virt % PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys % PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys_end % PAGE_SIZE) {
        res = -EINVARG;
        goto out;
    }
    if ((uint32_t)phys_end < (uint32_t)phys) {
        res = -EINVARG;
        goto out;
    }

    uint32_t total_bytes = phys_end - phys;
    int total_pages = total_bytes / PAGE_SIZE;
    res = paging_map_range(directory, virt, phys, total_pages, flags);
out:
    return res;
}