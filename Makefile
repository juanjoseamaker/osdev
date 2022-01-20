CC 			= gcc
ASM 		= nasm

SOURCE_PATH	= ./kernel
BUILD_PATH	= ./build

CC_FLAGS	= -m32 -ffreestanding -Wall -Wextra -nostdlib -fno-stack-protector -c
ASM_FLAGS	= -f elf

CFILES    := $(sort $(shell find $(SOURCE_PATH) -type f -name '*.c'))
COBJFILES := $(patsubst %.c,$(BUILD_PATH)/%.o,$(CFILES))

ASMFILES  := $(sort $(shell find $(SOURCE_PATH) -type f -name '*.asm'))
ASMOBJFILES := $(patsubst %.asm,$(BUILD_PATH)/%.o,$(ASMFILES))

$(BUILD_PATH)/%.o: %.c
	mkdir -p ${dir $@}
	$(CC) $(CC_FLAGS) $< -o $@

$(BUILD_PATH)/%.o: %.asm
	mkdir -p ${dir $@}
	$(ASM) $(ASM_FLAGS) $< -o $@

OS.iso: OS.elf
	grub-file --is-x86-multiboot $(BUILD_PATH)/OS.elf

	mkdir -p isodir/boot/grub
	cp $(BUILD_PATH)/OS.elf isodir/boot/OS.elf
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o OS.iso isodir

OS.elf: $(COBJFILES) $(ASMOBJFILES)
	ld -m elf_i386 -T linker.ld $(COBJFILES) $(ASMOBJFILES) -o $(BUILD_PATH)/OS.elf -nostdlib

run_grub: OS.iso
	qemu-system-i386 -cdrom OS.iso -enable-kvm -serial stdio -soundhw pcspk

run: OS.elf
	qemu-system-i386 -kernel $(BUILD_PATH)/OS.elf -enable-kvm -serial stdio -soundhw pcspk

clean:
	rm -r $(COBJFILES) $(ASMOBJFILES) $(BUILD_PATH)/OS.elf OS.iso isodir
