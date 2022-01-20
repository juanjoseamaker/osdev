#include "framebuffer.h"
#include "../kernel.h"

void print_framebuffer_info(framebuffer_t *fb) {
    printk("DEBUG: framebuffer addr = %x | size = %u:%u | pitch = %u | bpp = %u | rmf-gmf-bmf = %u:%u:%u:%u:%u:%u\n",
        (uint32_t) fb->addr,
        fb->width,
        fb->height,
        fb->pitch,
        (uint32_t) fb->bpp,
        (uint32_t) fb->red_mask_size,
        (uint32_t) fb->red_field_position,
        (uint32_t) fb->green_mask_size,
        (uint32_t) fb->green_field_position,
        (uint32_t) fb->blue_mask_size,
        (uint32_t) fb->blue_field_position
    );
}

void framebuffer_set_pixel(framebuffer_t *fb, uint32_t x, uint32_t y, uint32_t color) {
    if(x < fb->width && y < fb->height) {
        uint32_t *pixel = fb->addr + fb->pitch * y + (fb->bpp / 8) * x;
	    *pixel = color;
    }
}

uint32_t framebuffer_color(framebuffer_t *fb, uint32_t red, uint32_t green, uint32_t blue) {
    uint32_t color = 0;
    if(red)
        color |= ((1 << fb->red_mask_size) - 1) << fb->red_field_position;
    if(green)
        color |= ((1 << fb->green_mask_size) - 1) << fb->green_field_position;
    if(blue)
        color |= ((1 << fb->blue_mask_size) - 1) << fb->blue_field_position;
    
    return color;
}
