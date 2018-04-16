all:loader.bin mbr.bin

loader.bin:loader.S
	nasm -I ../include/ loader.S -o loader.bin
	dd if=loader.bin of=../hd60M.img bs=512 count=4 seek=2 conv=notrunc
mbr.bin:mbr.S
	nasm mbr.S -o mbr.bin
	dd if=mbr.bin of=../hd60M.img bs=512 count=1 conv=notrunc
clean:
	rm *.bin
