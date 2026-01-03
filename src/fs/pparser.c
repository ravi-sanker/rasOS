#include "pparser.h"
#include "string/string.h"
#include "status.h"
#include "kernel.h"
#include "memory/memory.h"
#include "stdbool.h"
#include "memory/heap/kheap.h"

// A valid path will be something like 0:/dir_name/file_name
static bool is_valid_path(const char* path) {
    int len = strlen(path);
    if (len > RASOS_PATH_MAX_LEN || len < 3) {
        return false;
    }
    if (!is_digit(path[0])) {
        return false;
    }
    if (memcmp((void*)&path[1], ":/", 2) != 0) {
        return false;
    }
    
    return true;
}

// get_drive_from_path returns the drive number and repositions path to skip the 
// drive part of the path.
static int get_drive_from_path(const char** path) {
    if (!is_valid_path(*path)) {
        return -EBADPATH;
    }

    int drive_number = *path[0] - '0';
    *path += 3;
    return drive_number;
}

static struct path_root* create_root(int drive_number) {
    struct path_root* path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_number = drive_number;
    path_r->first = 0;
    return path_r;
}

static const char* get_next_path_part(const char** path) {
    char* result_path_part = kzalloc(RASOS_PATH_MAX_LEN);
    int i = 0;
    while(**path != '/' && **path != 0) {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }
    if(**path == '/') {
        *path += 1;
    }
    if (i == 0) {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

struct path_part* parse_path_part(struct path_part* last_part, const char** path) {
    const char* path_part_name = get_next_path_part(path);
    if (!path_part_name) {
        return 0;
    }

    struct path_part* part = kzalloc(sizeof(struct path_part));
    part->part = path_part_name;
    part->next = 0;

    if (last_part) {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(struct path_root* root) {
    struct path_part* part = root->first;
    while(part) {
        struct path_part* next_part = part->next;
        kfree((void*) part->part);
        kfree(part);
        part = next_part;
    }

    kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path) {
    int drive_number = 0;
    const char* tmp_path = path;
    struct path_root* path_root = 0;

    if (strlen(path) > RASOS_PATH_MAX_LEN) {
        return 0;
    }

    drive_number = get_drive_from_path(&tmp_path);
    if (drive_number < 0) {
        return 0;
    }

    path_root = create_root(drive_number);
    if (!path_root) {
        return 0;
    }

    struct path_part* first_part = parse_path_part(0, &tmp_path);
    if (!first_part) {
        return 0;
    }

    path_root->first = first_part;
    struct path_part* part = parse_path_part(first_part, &tmp_path);
    while(part) {
        part = parse_path_part(part, &tmp_path);
    }
    return path_root;
}