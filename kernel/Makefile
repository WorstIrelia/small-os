all:kernel
	
kernel:main.o interrupt.o print.o kernel.o debug.o string.o memory.o bitmap.o
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o kernel main.o kernel.o string.o memory.o bitmap.o debug.o interrupt.o print.o 
	dd if=kernel of=../hd60M.img bs=512 count=200 seek=9 conv=notrunc
	
main.o:main.c
	gcc -c -m32 main.c -o main.o -fno-builtin
kernel.o:kernel.S
	nasm -f elf kernel.S -o kernel.o
print.o:print.S
	nasm -f elf print.S -o print.o
interrupt.o:interrupt.c
	gcc -c -m32 interrupt.c -o interrupt.o -fno-stack-protector
debug.o:debug.c
	gcc -c -m32 debug.c -o debug.o
string.o:string.c
	gcc -c -m32 string.c -o string.o -fno-builtin
bitmap.o:bitmap.c
	gcc -c -m32 bitmap.c -o bitmap.o -fno-builtin
memory.o:memory.c
	gcc -c -m32 memory.c -o memory.o -fno-builtin
clean:
	rm *.o kernel
