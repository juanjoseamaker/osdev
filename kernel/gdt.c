#include <stdint.h>

#include "gdt.h"

#define N_ENTRIES 3

extern void set_gdt(void *gdt, uint32_t size);
extern void reload_segments();

struct GDT_entry GDT[N_ENTRIES] = {
	{0, 0, 0},	// Selector 0x00 cannot be used
	{0, 0xffffffff, 0x9A},	// Selector 0x08 will be our code
	{0, 0xffffffff, 0x92}	// Selector 0x10 will be our data
};

uint8_t GDT_buffer[N_ENTRIES * 8];

void encode_gdt_entry(uint8_t *target, struct GDT_entry source) {
	if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF)) {
		return; /* TODO: panic here */
	}

	if (source.limit > 65536) {
		source.limit = source.limit >> 12;
		target[6] = 0xC0;
	} else {
		target[6] = 0x40;
	}
	 
	target[0] = source.limit & 0xFF;
	target[1] = (source.limit >> 8) & 0xFF;
	target[6] |= (source.limit >> 16) & 0xF;
	 
	target[2] = source.base & 0xFF;
	target[3] = (source.base >> 8) & 0xFF;
	target[4] = (source.base >> 16) & 0xFF;
	target[7] = (source.base >> 24) & 0xFF;
	 
	target[5] = source.type;
}

void gdt_init() {
	for(int i = 0; i < N_ENTRIES; i++) {
		encode_gdt_entry(&GDT_buffer[i * 8], GDT[i]);
	}

	set_gdt(GDT_buffer, sizeof(GDT_buffer));
	reload_segments();
}
