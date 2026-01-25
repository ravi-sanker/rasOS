FILES=./build/start.o ./build/rasos.o
INCLUDES=

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./stdlib.elf
	
./build/start.o: ./src/start.asm
	nasm -f elf ./src/start.asm -o ./build/start.o

./build/rasos.o: ./src/rasos.asm
	nasm -f elf ./src/rasos.asm -o ./build/rasos.o
clean:
	rm -rf ${FILES}
	rm -f ./stdlib.elf