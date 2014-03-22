CC := i586-elf-gcc
AS := i586-elf-as
LD := i586-elf-ld

WARNINGS := -Wall -Werror -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wuninitialized -Wconversion -Wstrict-prototypes 
CFLAGS := -g -ffreestanding $(WARNINGS) -std=c99

#based on 'Recursive Make Considered Harmful'
MODULES := interrupts kernel koutput memory
CFLAGS += $(patsubst %, -I%, $(MODULES))
SRC := 
include $(patsubst %,%/module.mk,$(MODULES))
OBJ := 	$(patsubst %.c,%.o,$(filter %.c,$(SRC))) \
	$(patsubst %.s,%.o,$(filter %.s,$(SRC)))

DISK := floppy.img
LOOP := /dev/loop0
MNT  := /mnt/floppy

.PHONY: all install

all: kernel.bin

kernel.bin: linker.ld $(OBJ)
	$(LD) -T linker.ld -o $@ $(OBJ)

%.d: %.c
	$(CC) $(CFLAGS) -MM -MF $@ $<

include $(patsubst %.c,%.d,$(filter %.c,$(SRC)))

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.s
	$(AS) -o $@ $<

install:
	@losetup $(LOOP) $(DISK); \
	mount $(LOOP) $(MNT); \
	cp ./kernel.bin $(MNT)/boot/; \
	umount $(LOOP); \
	losetup -d $(LOOP)
