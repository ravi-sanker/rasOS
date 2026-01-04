#include "fs/file.h"
#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"

struct disk disk;

// Read 'total' sectors from LBA into buffer.
int disk_read_sector(int lba, int total, void* buffer) {
    // Ref: https://wiki.osdev.org/ATA_PIO_Mode#28_bit_PIO
    // But don't bother going into the specifics. This is simply a contract.
    outb(0x1F6, (lba >> 24 | 0xE0));
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xFF));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*)buffer;
    for(int b = 0; b < total; b++) {
        char c = insb(0x1F7);
        while(!(c & 0x08)) {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory.
        for(int i = 0; i < 256; i++) {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

void disk_search_and_init() {
    memset(&disk, 0, sizeof(disk));
    disk.type = RASOS_DISK_TYPE_REAL;
    disk.sector_size = RASOS_SECTOR_SIZE;
    disk.filesystem = fs_resolve(&disk);
}

struct disk* disk_get(int index) {
    if (index != 0) {
        return 0;
    }
    return &disk;
}

int disk_read_block(struct disk* in_disk, unsigned int lba, int total, void* buffer) {
    if (in_disk != &disk) {
        return -EIO;
    }

    return disk_read_sector(lba, total, buffer);
}