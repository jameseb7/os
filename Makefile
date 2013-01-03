CC := i586-elf-gcc
AS := i586-elf-as
LD := i586-elf-ld

WARNINGS := -Wall -Werror -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wconversion -Wstrict-prototypes 
CFLAGS := -g -ffreestanding $(WARNINGS)

OBJECTS := loader.o kernel.o output.o gdt.o idt.o memory.o

DISK := floppy.img
LOOP := /dev/loop0
MNT  := /mnt/floppy



.PHONY: all install

all: kernel.bin

kernel.bin: linker.ld $(OBJECTS)
	@$(LD) -T linker.ld -o $@ $(OBJECTS)

loader.o: loader.s
	@$(AS) -o $@ $<
%.o: %.c
	@$(CC) $(CFLAGS) -o $@ -c $<

install:
	@losetup $(LOOP) $(DISK); \
	mount $(LOOP) $(MNT); \
	cp ./kernel.bin $(MNT)/boot/; \
	umount $(LOOP); \
	losetup -d $(LOOP)
