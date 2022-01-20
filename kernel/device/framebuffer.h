#pragma once

#include <stdint.h>

// RGB framebuffer

typedef struct {
    void *addr;

    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;

    uint8_t red_mask_size;
    uint8_t red_field_position;
    uint8_t green_mask_size;
    uint8_t green_field_position;
    uint8_t blue_mask_size;
    uint8_t blue_field_position;
} framebuffer_t;

typedef uint32_t color_t;

void print_framebuffer_info(framebuffer_t *fb);
void framebuffer_set_pixel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t color);
uint32_t framebuffer_color(framebuffer_t *fb, uint32_t red, uint32_t green, uint32_t blue);