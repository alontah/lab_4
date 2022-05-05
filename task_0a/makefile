all: exec

libs: asm-lib

asm-lib: start.s
	nasm -f elf32 start.s -o start.o

exec: main.c libs
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector util.c -o util.o
	gcc -m32 -Wall -ansi -c -nostdlib -fno-stack-protector main.c -o main.o
	ld -m elf_i386 start.o main.o util.o -o task0
	rm main.o util.o start.o 


.PHONY: clean
clean:
	rm -rf ./*.o main
