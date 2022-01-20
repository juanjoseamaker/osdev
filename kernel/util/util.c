#include "util.h"

#include <stdint.h>
#include <stddef.h>

void *memset(void *addr, int c, size_t len) {
	uint8_t *p = addr;

	while(len > 0) {
		*p = (uint8_t) c;
		p++;
		len--;
	}

	return addr;
}
