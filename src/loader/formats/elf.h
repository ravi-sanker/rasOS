#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

// Refer to the intro chapters of "Practical Binary Analysis".
// ELF binaries really consist of only four types of components: 
// - an executable header, 
// - a series of (optional) program headers, 
// - a number of sections, 
// - and a series of (optional) section headers, one per section.

// section header table gives the "section" view of the binary, used in static linking
// program header tables gives the "segment" view of the binary, used in dynamic linking
// a segment is zero or more sections, only needed for executable ELF files

typedef uint16_t    elf32_half;
typedef uint32_t    elf32_word;
typedef int32_t     elf32_sword;
typedef uint32_t    elf32_addr;
typedef uint32_t    elf32_off;

#define EI_NIDENT       16
#define EI_CLASS        4   // index of the class byte
#define EI_DATA         5   // index of the data byte
// The "class" byte is stored in e_ident in the byte just after the magic value.
#define ELFCLASSNONE    0
#define ELFCLASS32      1   // will support this
#define ELFCLASS64      2
// The "data" byte is stored after the "class" byte.
#define ELFDATANONE     0
#define ELFDATA2LSB     1   // will support this
#define ELFDATA2MSB     2
// This is for the e_type value in the elf header.
#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2
#define ET_DYN          3
#define ET_CORE         4

struct elf_header {
    unsigned char e_ident[EI_NIDENT]; // should start with 0x7f, e, l, f.
    elf32_half  e_type;     // ET_REL, ET_EXEC, ET_DYN...
    elf32_half  e_machine;
    elf32_word  e_version;  // EV_CURRENT (1)
    elf32_addr  e_entry;    // virt. address where the execution should start
    elf32_off   e_phoff;    // offset to program header table
    elf32_off   e_shoff;    // offset to section header table (can be 0)
    elf32_word  e_flags;    // set to 0 for x86
    elf32_half  e_ehsize;   // size of this header - 52 for x86 32 bit
    // size of program/section entries and the count for each
    elf32_half  e_phentsize;
    elf32_half  e_phnum;
    elf32_half  e_shentsize;
    elf32_half  e_shnum;
    elf32_half  e_shstrndx; // index in the section header table of the "string table section"
} __attribute__((packed));

//------------------------------------------------------------------------------

#define SHN_UNDEF 0

#define SHT_NULL        0
#define SHT_PROGBITS    1   // section contains program data
#define SHT_SYMTAB      2   // static symbol table
#define SHT_STRTAB      3   // string table
#define SHT_RELA        4   // contains relocation info, used in static linking
#define SHT_HASH        5
#define SHT_DYNAMIC     6   // contains info needed for dynamic linking
#define SHT_NOTE        7
#define SHT_NOBITS      8
#define SHT_REL         9   // contains relocation info, used in static linking
#define SHT_SHLIB       10
#define SHT_DYNSYM      11  // symbol table used by dynamic linker
#define SHT_LOPROC      12
#define SHT_HIPROC      13
#define SHT_LOUSER      14
#define SHT_HIUSER      15

struct elf32_shdr {
    elf32_word  sh_name;    // index into the string table
    elf32_word  sh_type;    // check STH_* definitions
    elf32_word  sh_flags;
    elf32_addr  sh_addr;
    elf32_off   sh_offset;
    elf32_word  sh_size;
    elf32_word  sh_link;
    elf32_word  sh_info;
    elf32_word  sh_addralign;
    elf32_word  sh_entsize;
} __attribute__((packed));

//------------------------------------------------------------------------------

// This is for the p_flags field in program header.
#define PF_X        0x01
#define PF_W        0x02
#define PF_R        0x04
// This is for the p_type field in program header.
#define PT_NULL     0
#define PT_LOAD     1
#define PT_DYNAMIC  2
#define PT_INTERP   3
#define PT_NOTE     4
#define PT_SHLIB    5
#define PT_PHDR     6

struct elf32_phdr {
    elf32_word  p_type;     // defines the type of the segment
    elf32_off   p_offset;
    elf32_addr  p_vaddr;
    elf32_addr  p_paddr;
    elf32_word  p_filesz;
    elf32_word  p_memsz;
    elf32_word  p_flags;    // runtime permission for the segment
    elf32_word  p_align;
} __attribute__((packed));


//------------------------------------------------------------------------------

struct elf32_dyn {
    elf32_sword d_tag;
    union {
        elf32_word d_val;
        elf32_addr d_ptr;
    } d_un;
} __attribute__((packed));

struct elf32_sym {
    elf32_word st_name;
    elf32_addr st_value;
    elf32_word st_size;
    unsigned char st_info;
    unsigned char st_other;
    elf32_half st_shndx;
} __attribute__((packed));

void* elf_get_entry_ptr(struct elf_header* elf_header);
uint32_t elf_get_entry(struct elf_header* elf_header);

#endif
