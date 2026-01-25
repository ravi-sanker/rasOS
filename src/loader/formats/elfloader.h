#ifndef ELFLOADER_H
#define ELFLOADER_H

#include <stdint.h>
#include <stddef.h>

#include "elf.h"
#include "config.h"
#include "kernel.h"

struct elf_file {
    char filename[RASOS_PATH_MAX_LEN];

    int in_memory_size;

    // The physical address of the starting of elf file.
    void* elf_memory;

    // The virtual/physical addresses of the "executable" section of the elf file.
    void* virtual_base_address;
    void* virtual_end_address;
    void* physical_base_address;
    void* physical_end_address;
};

int elf_load(const char* filename, struct elf_file** file_out);
void elf_close(struct elf_file* file);
void* elf_virtual_base(struct elf_file* file);
void* elf_virtual_end(struct elf_file* file);
void* elf_phys_base(struct elf_file* file);
void* elf_phys_end(struct elf_file* file);
struct elf_header* elf_header(struct elf_file* file);
struct elf32_shdr* elf_sheader(struct elf_header* header);
void* elf_memory(struct elf_file* file);
struct elf32_phdr* elf_pheader(struct elf_header* header);
struct elf32_phdr* elf_program_header(struct elf_header* header, int index);
struct elf32_shdr* elf_section(struct elf_header* header, int index);
void* elf_phdr_phys_address(struct elf_file* file, struct elf32_phdr* phdr);

#endif
