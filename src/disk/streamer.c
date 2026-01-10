#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"
#include <stdbool.h>

struct disk_stream* diskstreamer_new(int disk_number) {
    struct disk* disk = disk_get(disk_number);
    if (!disk) {
        return 0;
    }

    struct disk_stream* streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int diskstreamer_seek(struct disk_stream* stream, int pos) {
    stream->pos = pos;
    return 0;
}

int diskstreamer_read(struct disk_stream* stream, void* out, int total) {
    char buf[RASOS_SECTOR_SIZE];
    int sector = stream->pos / RASOS_SECTOR_SIZE;
    int offset = stream->pos % RASOS_SECTOR_SIZE;
    bool is_overflow = (offset + total) >= RASOS_SECTOR_SIZE;
    int bytes_in_current_sector = total;
    if (is_overflow) {
        bytes_in_current_sector = RASOS_SECTOR_SIZE - offset;
    }

    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0) {
        return res;
    }

    for (int i = 0; i < bytes_in_current_sector; i++) {
        *(char*)out++ = buf[offset+i];
    }

    stream->pos += bytes_in_current_sector;
    if (is_overflow) {
        res = diskstreamer_read(stream, out, total-bytes_in_current_sector);
    }

    return res;
}

void diskstreamer_close(struct disk_stream* stream) {
    kfree(stream);
}