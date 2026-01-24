OBJECT_FILES = ./build/kernel.asm.o ./build/kernel.o ./build/idt/idt.asm.o \
	./build/memory/memory.o ./build/idt/idt.o ./build/io/io.asm.o \
	./build/memory/heap/heap.o ./build/memory/heap/kheap.o \
	./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o \
	./build/disk/disk.o ./build/disk/streamer.o ./build/fs/pparser.o \
	./build/string/string.o ./build/fs/file.o ./build/fs/fat/fat16.o \
	./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/task/tss.asm.o \
	./build/task/task.o ./build/task/process.o ./build/task/task.asm.o \
	./build/isr80h/isr80h.o ./build/isr80h/syscalls.o ./build/keyboard/keyboard.o \
	./build/keyboard/qwerty.o ./build/display/display.o ./build/loader/formats/elfloader.o \
	./build/loader/formats/elf.o

INCLUDES =  -I./src

C_FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels \
	-falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions \
	-Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp \
	-Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 \
	-Iinc

all: clean directories ./bin/boot.bin ./bin/kernel.bin user_programs
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
.PHONY: all

#-------------------------------------------------------------------------------
# Generate the binaries from object files.
#-------------------------------------------------------------------------------

# Create an intermediate object file first and then link it with the linker.
./bin/kernel.bin: $(OBJECT_FILES)
	i686-elf-ld -g -relocatable $(OBJECT_FILES) -o ./build/kernelfull.o
	i686-elf-gcc $(C_FLAGS) -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

# The bootloader binary depends only on the boot.asm file.
./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin ./src/boot/boot.asm -o ./bin/boot.bin

#-------------------------------------------------------------------------------
# Generate object files from c and assembly code.
# For clarity, object files generated from assembly are suffixed with .asm.o.
#-------------------------------------------------------------------------------

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(C_FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(C_FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(C_FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc $(INCLUDES) -I./src/heap $(C_FLAGS) -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc $(INCLUDES) -I./src/heap $(C_FLAGS) -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc $(INCLUDES) -I./src/paging $(C_FLAGS) -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g ./src/memory/paging/paging.asm -o ./build/memory/paging/paging.asm.o

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(C_FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/disk/streamer.o: ./src/disk/streamer.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(C_FLAGS) -std=gnu99 -c ./src/disk/streamer.c -o ./build/disk/streamer.o

./build/fs/pparser.o: ./src/fs/pparser.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(C_FLAGS) -std=gnu99 -c ./src/fs/pparser.c -o ./build/fs/pparser.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(C_FLAGS) -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c
	i686-elf-gcc $(INCLUDES) -I./src/fs $(C_FLAGS) -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc $(INCLUDES) -I./src/string $(C_FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc $(INCLUDES) -I./src/gdt $(C_FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o

./build/task/tss.asm.o: ./src/task/tss.asm
	nasm -f elf -g ./src/task/tss.asm -o ./build/task/tss.asm.o

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(C_FLAGS) -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc $(INCLUDES) -I./src/task $(C_FLAGS) -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

./build/task/task.asm.o: ./src/task/task.asm
	nasm -f elf -g ./src/task/task.asm -o ./build/task/task.asm.o

./build/isr80h/isr80h.o: ./src/isr80h/isr80h.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(C_FLAGS) -std=gnu99 -c ./src/isr80h/isr80h.c -o ./build/isr80h/isr80h.o

./build/isr80h/syscalls.o: ./src/isr80h/syscalls.c
	i686-elf-gcc $(INCLUDES) -I./src/isr80h $(C_FLAGS) -std=gnu99 -c ./src/isr80h/syscalls.c -o ./build/isr80h/syscalls.o

./build/keyboard/keyboard.o: ./src/keyboard/keyboard.c
	i686-elf-gcc $(INCLUDES) -I./src/keyboard $(C_FLAGS) -std=gnu99 -c ./src/keyboard/keyboard.c -o ./build/keyboard/keyboard.o

./build/keyboard/qwerty.o: ./src/keyboard/qwerty.c
	i686-elf-gcc $(INCLUDES) -I./src/keyboard $(C_FLAGS) -std=gnu99 -c ./src/keyboard/qwerty.c -o ./build/keyboard/qwerty.o

./build/display/display.o: ./src/display/display.c
	i686-elf-gcc $(INCLUDES) -I./src/display $(C_FLAGS) -std=gnu99 -c ./src/display/display.c -o ./build/display/display.o

./build/loader/formats/elf.o: ./src/loader/formats/elf.c
	i686-elf-gcc $(INCLUDES) -I./src/loader/formats $(C_FLAGS) -std=gnu99 -c ./src/loader/formats/elf.c -o ./build/loader/formats/elf.o

./build/loader/formats/elfloader.o: ./src/loader/formats/elfloader.c
	i686-elf-gcc $(INCLUDES) -I./src/loader/formats $(C_FLAGS) -std=gnu99 -c ./src/loader/formats/elfloader.c -o ./build/loader/formats/elfloader.o

#-------------------------------------------------------------------------------

user_programs:
	cd ./programs/blank && $(MAKE) all

user_programs_clean:
	cd ./programs/blank && $(MAKE) clean

#-------------------------------------------------------------------------------

directories:
	cd ./build && mkdir -p memory && mkdir -p idt && mkdir -p io && mkdir -p disk && mkdir -p isr80h && mkdir -p keyboard\
	&& mkdir -p display && mkdir -p gdt && mkdir -p task && mkdir -p fs && cd fs && mkdir -p fat && cd .. && mkdir -p string
	cd ./build/memory && mkdir -p heap && mkdir -p paging && cd .. && mkdir -p loader/formats && cd loader/formats && cd ../..
	cd programs/blank && mkdir -p build && cd ../..
	mkdir -p ./mnt/d && echo "Hello, World!" > hello.txt
.PHONY: directories

clean: user_programs_clean
	rm -rf ./bin/*
	rm -rf ./build/*

#-------------------------------------------------------------------------------

# Useful GDB commands:
# target remote | qemu-system-i386 -hda ./bin/os.bin -S -gdb stdio
# add-symbol-file ./build/kernelfull.o 0x100000
# break file_name:line_number
