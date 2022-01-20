#include <stdint.h>

#include "multiboot.h"
#include "gdt.h"
#include "device/vga.h"
#include "util/format.h"
#include "mem/pmm.h"
#include "kernel.h"
#include "device/serial.h"
#include "int/isr.h"
#include "device/keyboard.h"
#include "device/pit.h"
#include "device/framebuffer.h"
#include "device/mouse.h"

#define CHECK_FLAG(flags,bit) ((flags) & (1 << (bit)))

extern uint8_t *kernel_end;

framebuffer_t fb;

void main(uint32_t magic, uint32_t addr) {
	multiboot_info_t *mbi;

	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		return;
	}

	mbi = (multiboot_info_t *) addr;

	gdt_init();

	serial_enable(COM1);
	serial_enable(COM2);

	if(CHECK_FLAG(mbi->flags, 12)) {
		switch(mbi->framebuffer_type) {
		case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
			vga_init(mbi->framebuffer_width, mbi->framebuffer_height);
			printk("DEBUG: graphic mode = EGA_TEXT\n");
			break;
		case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
			printk("DEBUG: framebuffer addr = %x\n", (uint32_t)mbi->framebuffer_addr);
							
			printk("DEBUG: graphic mode = framebuffer indexed | pitch = %d bytes | bits per pixel = %d | size = %d:%d\n",
				mbi->framebuffer_pitch, mbi->framebuffer_bpp, mbi->framebuffer_width, mbi->framebuffer_height);
			panic("Unsupported graphic mode");
			break;
		case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
			printk("DEBUG: framebuffer addr = %x\n", (uint32_t)mbi->framebuffer_addr);
			
			printk("DEBUG: graphic mode = framebuffer rgb | pitch = %d bytes | bits per pixel = %d | size = %d:%d\n",
				mbi->framebuffer_pitch, mbi->framebuffer_bpp, mbi->framebuffer_width, mbi->framebuffer_height);
			
			break;
		default:
			panic("invalid graphic mode\n");
			return;
		}
	} else {
		vga_init(80, 24);
		printk("DEBUG: no graphic mode given\n");
	}

	printk("DEBUG: multiboot information addr = %x\n", (uint32_t) mbi);

	if(CHECK_FLAG(mbi->flags, 0) && CHECK_FLAG(mbi->flags, 6)) {
		pmm_init((uint32_t) &kernel_end, 1024 + mbi->mem_upper);
		print_memory_regions(mbi->mmap_addr, mbi->mmap_addr + mbi->mmap_length);
		pmm_init_available_regions(mbi->mmap_addr, mbi->mmap_addr + mbi->mmap_length);
		pmm_deinit_kernel();
	} else {
		panic("memory information not available\n");
		return;
	}

	if((void *)(uint32_t) mbi->framebuffer_addr != NULL && mbi->framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT) {
		printk("DEBUG: creating framebuffer device\n");

		fb.addr = (void *)(uint32_t) mbi->framebuffer_addr;
		fb.width = mbi->framebuffer_width;
		fb.height = mbi->framebuffer_height;
		fb.pitch = mbi->framebuffer_pitch;
		fb.bpp = mbi->framebuffer_bpp;
		fb.red_mask_size = mbi->framebuffer_red_mask_size;
		fb.red_field_position = mbi->framebuffer_red_field_position;
		fb.green_mask_size = mbi->framebuffer_green_mask_size;
		fb.green_field_position = mbi->framebuffer_green_field_position;
		fb.blue_mask_size = mbi->framebuffer_blue_mask_size;
		fb.blue_field_position = mbi->framebuffer_blue_field_position;

		print_framebuffer_info(&fb);

		pmm_deinit_region((uint32_t) mbi->framebuffer_addr, mbi->framebuffer_pitch * mbi->framebuffer_height);
		printk("DEBUG: framebuffer memory base=%x size=%x deinited\n", (uint32_t) mbi->framebuffer_addr, mbi->framebuffer_pitch * mbi->framebuffer_height);

		if(!is_block_used((void *)(uint32_t) mbi->framebuffer_addr))
			panic("framebuffer region is usable\n");
	}

	printk("DEBUG: installing and enabling external interrupts\n");
    isr_install();
    
	printk("DEBUG: initializating PIT (IRQ 0)\n");
	pit_init();

	printk("DEBUG: initializating keyboard (IRQ 1)\n");
	init_keyboard();

	printk("DEBUG: initializating mouse (IRQ 12)\n");
	init_mouse();

	asm volatile("sti");

	printk("DEBUG: booting stage completed\n");

	while(1) {
		sleep(5);

		uint32_t color = framebuffer_color(&fb, 0, 0, 0);
		if(mouse_buttons() & LEFT_BUTTON)
			color = framebuffer_color(&fb, 1, 1, 1);
			
		if(mouse_buttons() & RIGHT_BUTTON)
			for(uint32_t x = 0; x < fb.width; x++) {
				for(uint32_t y = 0; y < fb.width; y++) {
					framebuffer_set_pixel(&fb, x, y, framebuffer_color(&fb, 0, 0, 0));
				}
			}
			
		for(uint32_t x = 0; x < fb.width; x++) {
			for(uint32_t y = 0; y < fb.width; y++) {
				if((x-mouse_coord_x()) * (x-mouse_coord_x()) + (y-mouse_coord_y()) * (y-mouse_coord_y()) < 100)
					framebuffer_set_pixel(&fb, x, y, color);
			}
		}
	}
}
