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
#endif
