#pragma once

#include <stdint.h>

struct GDT_entry {
	uint32_t base;
	uint32_t limit;
	uint8_t type;
};

void gdt_init();
