#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

//------------------------------------------------------------------------------

static bool is_block_taken(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0b00000001;
}

static bool is_block_next_exists(HEAP_BLOCK_TABLE_ENTRY entry) {
    return entry & 0b10000000;
}

int heap_address_to_block(struct heap* heap, void* address) {
    return ((int)(address - heap->start_address)) / RASOS_HEAP_BLOCK_SIZE;
}

void* heap_block_to_address(struct heap* heap, int block) {
    return heap->start_address + (block * RASOS_HEAP_BLOCK_SIZE);
}

//------------------------------------------------------------------------------

int heap_create(struct heap* heap, void* table_start_addr, void* heap_start_addr, size_t heap_size) {
    int res = 0;

    if (((unsigned int)heap_start_addr % RASOS_HEAP_BLOCK_SIZE) != 0) {
        res = -EINVARG;
        goto out;
    }
    if (heap_size % RASOS_HEAP_BLOCK_SIZE != 0) {
        res = -EINVARG;
        goto out;
    }

    int n_blocks = heap_size / RASOS_HEAP_BLOCK_SIZE;
    memset(table_start_addr, HEAP_BLOCK_ENTRY_FREE, n_blocks);
    heap->table->entries = table_start_addr;
    heap->table->total = n_blocks;

    // The following commented line is resulting in a panic. Need to debug this.
    // memset(heap_start_addr, 0, heap_size);

    heap->start_address = heap_start_addr;

out:
    return res;
}

//------------------------------------------------------------------------------

static uint32_t heap_size_ceiling(uint32_t heap_size) {
    if ((heap_size % RASOS_HEAP_BLOCK_SIZE) == 0) {
        return heap_size;
    }

    heap_size = (heap_size - ( heap_size % RASOS_HEAP_BLOCK_SIZE));
    heap_size += RASOS_HEAP_BLOCK_SIZE;
    return heap_size;
}

int heap_get_start_block(struct heap* heap, uint32_t total_blocks) {
    struct heap_table* table = heap->table;
    int block_count = 0;
    int block_start_index = -1;

    for (size_t i = 0; i < table->total; i++) {
        if (is_block_taken(table->entries[i])) {
            block_count = 0;
            block_start_index = -1;
            continue;
        }

        if (block_start_index == -1) {
            block_start_index = i;
        }

        block_count++;
        if (block_count == total_blocks) {
            return block_start_index;
        }
    }

    return -ENOMEM;
}

void heap_mark_blocks_taken(struct heap* heap, int start_block, int total_blocks) {
    int end_block = start_block + total_blocks -1;
    
    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_ENTRY_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++) {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_ENTRY_TAKEN;
        if (i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void* heap_malloc_blocks(struct heap* heap, uint32_t total_blocks) {
    void* address = 0;

    int start_block = heap_get_start_block(heap, total_blocks);
    if (start_block < 0) {
        goto out;
    }

    heap_mark_blocks_taken(heap, start_block, total_blocks);
    
    address = heap_block_to_address(heap, start_block);

out:
    return address;
}

void* heap_malloc(struct heap* heap, size_t size) {
    size_t aligned_size = heap_size_ceiling(size);
    uint32_t total_blocks = aligned_size / RASOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

//------------------------------------------------------------------------------

void heap_mark_blocks_free(struct heap* heap, int starting_block) {
    struct heap_table* table = heap->table;
    for (int i = starting_block; i < (int)table->total; i++) {
        table->entries[i] = HEAP_BLOCK_ENTRY_FREE;
        if (!is_block_next_exists(table->entries[i])){
            break;
        }
    }
}

void heap_free(struct heap* heap, void* ptr) {
    heap_mark_blocks_free(heap, heap_address_to_block(heap, ptr));
}
