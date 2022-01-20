#pragma once

#include <stddef.h>
#include <stdint.h>

#define BLOCK_SIZE 4096
#define KB_TO_BLOCKS(x) (((x) * 1024) / BLOCK_SIZE)
#define IS_ALIGNED(addr, align) !((addr) & ~((align) - 1))
#define ALIGN(addr, align) (((addr) & ~((align) - 1)) + (align))

#define BLOCKS_PER_DWORD 32

void pmm_init(uint32_t pmmap_addr, size_t size);
void pmm_init_available_regions(uint32_t mmap_, uint32_t mmap_end_);
void pmm_deinit_kernel(void);
void *pmm_alloc_block(void);
void pmm_free_block(void *p);

void pmm_init_region(uint32_t base, size_t size);
void pmm_deinit_region(uint32_t base, size_t size);

void print_memory_regions(uint32_t mmap_, uint32_t mmap_end_);

int is_block_used(void *p);
