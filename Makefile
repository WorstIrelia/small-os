
CC=gcc
LIB= -I lib/ -I kernel/ -I device/ -I boot/ -I thread/ -I boot/include/
CFLAGS= -m32 -fno-builtin $(LIB) -fno-stack-protector -c

NASM=nasm
NLIB= -I boot/include/
NFLAGS=-f elf $(NLIB)

ALLOBJS=build/main.o build/kernel.o build/thread.o build/list.o build/switch.o\
build/string.o build/memory.o build/timer.o build/bitmap.o build/debug.o \
build/interrupt.o build/print.o 
all:build/mbr build/loader build/kernel
build/kernel:${ALLOBJS}
	ld -m elf_i386 -Ttext 0xc0001500 -e main -o $@ $^
	dd if=build/kernel of=hd60M.img bs=512 count=200 seek=9 conv=notrunc
build/kernel:${ALLOBJS}
build/main.o:kernel/main.c
	$(CC) $(CFLAGS) $< -o $@
build/timer.o:device/timer.c
	$(CC) $(CFLAGS) $< -o $@
build/thread.o:thread/thread.c
	$(CC) $(CFLAGS) $< -o $@
build/debug.o:lib/debug.c
	$(CC) $(CFLAGS) $< -o $@
build/string.o:lib/string.c 
	$(CC) $(CFLAGS) $< -o $@
build/list.o:kernel/list.c
	$(CC) $(CFLAGS) $< -o $@
build/bitmap.o:kernel/bitmap.c
	$(CC) $(CFLAGS) $< -o $@
build/interrupt.o:kernel/interrupt.c
	$(CC) $(CFLAGS) $< -o $@
build/memory.o:kernel/memory.c
	$(CC) $(CFLAGS) $< -o $@
build/kernel.o:kernel/kernel.S
	$(NASM) $(NFLAGS) $< -o $@
build/print.o:lib/print.S
	$(NASM) $(NFLAGS) $< -o $@
build/mbr:boot/mbr.S
	$(NASM) $(NLIB) $< -o $@ 
	dd if=build/mbr of=hd60M.img bs=512 count=1 conv=notrunc
build/kernel:${ALLOBJS}
build/loader:boot/loader.S
	$(NASM) $(NLIB) $< -o $@
	dd if=build/loader of=hd60M.img bs=512 count=4 seek=2 conv=notrunc
build/kernel:${ALLOBJS}
build/switch.o:thread/switch.S
	$(NASM) $(NFLAGS) $< -o $@
clean:
	cd build && rm -f ./*
