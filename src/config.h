#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define RASOS_TOTAL_INTERRUPTS 512

// TODO: Determine this from the system the OS is running on.
#define RASOS_HEAP_SIZE_BYTES   104857600   // 100MB heap size. 
#define RASOS_HEAP_BLOCK_SIZE   4096        // Each block is 4096 bytes.

// Reference: https://wiki.osdev.org/Memory_Map_(x86)
#define RASOS_HEAP_ADDRESS          0x01000000 
#define RASOS_HEAP_TABLE_ADDRESS    0x00007E00

#define RASOS_SECTOR_SIZE 512

#define RASOS_MAX_FILESYSTEMS       12
#define RASOS_MAX_FILEDESCRIPTORS   512

#define RASOS_TOTAL_GDT_SEGMENTS 6

#define RASOS_PROGRAM_VIRTUAL_ADDRESS               0x400000  // this is just chosen by convention
#define RASOS_USER_PROGRAM_STACK_SIZE               1024 * 16
#define RASOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START   0x3FF000
#define RASOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END     RASOS_PROGRAM_VIRTUAL_ADDRESS_START - RASOS_USER_PROGRAM_STACK_SIZE

// These are defined based on the GDT offset for these segments.
#define USER_DATA_SEGMENT 0x23
#define USER_CODE_SEGMENT 0x1b

#endif
