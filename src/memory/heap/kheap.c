#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"

struct  heap        kernel_heap;
struct  heap_table  kernel_heap_table;

void kheap_init() {
    kernel_heap.table = &kernel_heap_table;
    int res = heap_create(&kernel_heap, (void*)RASOS_HEAP_TABLE_ADDRESS, (void*)RASOS_HEAP_ADDRESS, RASOS_HEAP_SIZE_BYTES);
    if (res < 0) {
        terminal_print("Failed to create heap\n");
    }
}

void* kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void kfree(void* ptr) {
    heap_free(&kernel_heap, ptr);
}
