all:kernel
	
kernel:main.o
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o kernel main.o
	dd if=kernel of=../hd60M.img bs=512 count=200 seek=9 conv=notrunc
	
main.o:main.c
	gcc -c -m32 main.c -o main.o
clean:
	rm *.o kernel
