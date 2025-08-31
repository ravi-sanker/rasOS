#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

// We use the same bit masks for both directory and table entries as the 
// same flags are used till the 5th index. 
#define PAGING_CACHE_DISABLE    0b00010000
#define PAGING_WRITE_THROUGH    0b00001000
#define PAGING_ACCESS_BY_ALL    0b00000100
#define PAGING_IS_WRITABLE      0b00000010
#define PAGING_IS_PRESENT       0b00000001

#define PAGE_DIRECTORY_TOTAL_ENTRIES    1024
#define PAGE_TABLE_TOTAL_ENTRIES        1024
#define PAGE_SIZE                       4096

struct paging_4gb_chunk {
    uint32_t* directory_entry;
};

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
void paging_switch(uint32_t* directory);
void enable_paging();

#endif
