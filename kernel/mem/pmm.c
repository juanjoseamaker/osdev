#include <stddef.h>
#include <stdint.h>

#include "pmm.h"
#include "../util/util.h"
#include "../multiboot.h"
#include "../kernel.h"

static size_t mem_size;
static size_t max_blocks;
static size_t used_blocks;
static uint32_t *pmmap;
static size_t pmmap_size;

static void bitmap_set(uint32_t *bitmap, uint32_t bit) {
	bitmap[bit/BLOCKS_PER_DWORD] |= 1UL << (bit%BLOCKS_PER_DWORD);
}

static void bitmap_unset(uint32_t *bitmap, uint32_t bit) {
	bitmap[bit/BLOCKS_PER_DWORD] &= ~(1UL << (bit%BLOCKS_PER_DWORD));
}

static uint8_t bitmap_check(uint32_t *bitmap, uint32_t bit) {
	return (bitmap[bit/BLOCKS_PER_DWORD] >> (bit%BLOCKS_PER_DWORD)) & 1U;
}

static int bitmap_first_unset(uint32_t *bitmap, size_t max) {
	for(size_t i = 0; i < max; i++) {
		if(!bitmap_check(bitmap, i)) {
			return (int) i;
		}
	}

	return -1;
}

void pmm_init(uint32_t pmmap_addr, size_t size) {
	mem_size = size;
	max_blocks = KB_TO_BLOCKS(mem_size);
	used_blocks = max_blocks;
	pmmap = (uint32_t *) pmmap_addr;

	pmmap_size = max_blocks / BLOCKS_PER_DWORD;
	if(max_blocks % BLOCKS_PER_DWORD)
		pmmap_size++;

	memset(pmmap, 0xFF, pmmap_size);
}

void pmm_init_region(uint32_t base, size_t size) {
	size_t blocks  = size / BLOCK_SIZE;
	uint32_t align = base / BLOCK_SIZE;
	
	for (size_t i = 0; i < blocks; i++) {	
		bitmap_unset(pmmap, align++);
		used_blocks--;
	}
	
	bitmap_set(pmmap, 0);
}

void pmm_deinit_region(uint32_t base, size_t size) {
    size_t blocks = size / BLOCK_SIZE;
    uint32_t align = base / BLOCK_SIZE;
    
    for (size_t i = 0; i < blocks; i++) {
        bitmap_set(pmmap, align++);
        used_blocks++;
    }
}

void pmm_init_available_regions(uint32_t mmap_, uint32_t mmap_end_) {
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_;
    multiboot_memory_map_t *mmap_end = (multiboot_memory_map_t *) mmap_end_;

    for (int i = 0; mmap < mmap_end; mmap++, i++)
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
            pmm_init_region((uint32_t) mmap->addr, (size_t) mmap->len);
}

void print_memory_regions(uint32_t mmap_, uint32_t mmap_end_) {
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *) mmap_;
    multiboot_memory_map_t *mmap_end = (multiboot_memory_map_t *) mmap_end_;

    for (int i = 0; mmap < mmap_end; mmap++, i++) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
        	printk("DEBUG: memory region addr=%x len=%x is available\n", (uint32_t) mmap->addr, (uint32_t) mmap->len);
       	} else {
       		printk("DEBUG: memory region addr=%x len=%x is not available\n", (uint32_t) mmap->addr, (uint32_t) mmap->len);
       	}
	}
}

void pmm_deinit_kernel(void) {
    extern uint8_t *kernel_start;
    extern uint8_t *kernel_end;

    size_t kernel_size = (size_t) &kernel_end - (size_t) &kernel_start;

    uint32_t pmmap_size_aligned = pmmap_size;
    if (!IS_ALIGNED(pmmap_size_aligned, BLOCK_SIZE))
        pmmap_size_aligned = ALIGN(pmmap_size_aligned, BLOCK_SIZE);

    pmm_deinit_region((uint32_t) &kernel_start, kernel_size);
    pmm_deinit_region((uint32_t) &kernel_end, pmmap_size_aligned);
}

void *pmm_alloc_block(void) {
    int p_index = bitmap_first_unset(pmmap, max_blocks);

    if (p_index == -1)
        return NULL;

    bitmap_set(pmmap, p_index);
    used_blocks++;

    return (void *) (BLOCK_SIZE * p_index);
}

void pmm_free_block(void *p) {
    if (p == NULL)
        return;

    uint32_t p_addr = (uint32_t) p;

    int index = p_addr / BLOCK_SIZE;
    bitmap_unset(pmmap, index);

    used_blocks--;
}

int is_block_used(void *p) {
	if (p == NULL)
		return 1;

	uint32_t p_addr = (uint32_t) p;
	int index = p_addr / BLOCK_SIZE;

	return bitmap_check(pmmap, index);
}
