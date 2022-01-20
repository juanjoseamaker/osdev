#pragma once

#include <stdint.h>

void vga_puts(char *string);
void vga_putc(char ch);
void vga_set_color(char background, char foreground);
void vga_init(uint32_t width, uint32_t height);

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED	4
#define MAGENTA 5
#define BROWN 6
#define WHITE 7
#define DARK_GRAY 8
#define BRIGHT_BLUE 9
#define BRIGHT_GREEN 10
#define BRIGHT_CYAN 11
#define PINK 12
#define BRIGHT_MAGENTA 13
#define YELLOW 14
#define BRIGHT_WHITE 15
