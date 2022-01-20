#include "vga.h"
#include "port.h"

#define VGA_CTRL_REGISTER 0x3d4
#define VGA_DATA_REGISTER 0x3d5
#define VGA_OFFSET_LOW 0x0f
#define VGA_OFFSET_HIGH 0x0e

#define VIDEO_ADDRESS 0xb8000

static int max_rows;
static int max_cols;
static char vga_color = 0x0f;

void vga_set_color(char background, char foreground) {
	vga_color = background << 4 | foreground;
}

static void set_cursor(int offset) {
	port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
	port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset >> 8));
	port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
	port_byte_out(VGA_DATA_REGISTER, (unsigned char) (offset & 0xff));
}

static int get_cursor() {
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
    int offset = port_byte_in(VGA_DATA_REGISTER) << 8;
    port_byte_out(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
    offset += port_byte_in(VGA_DATA_REGISTER);
    return offset;
}

static void set_char(int offset, char character) {
    unsigned char *video_memory = (unsigned char *) VIDEO_ADDRESS;
    video_memory[offset * 2] = character;
    video_memory[offset * 2 + 1] = vga_color;
}

static void clean_screen() {
    for(int i = 0; i < max_cols * max_rows; i++) {
        set_char(i, 0);
    }
}

void vga_putc(char ch) {
	int offset = get_cursor();

	if(offset > max_cols * max_rows) {
		clean_screen();
		set_cursor(0);
		offset = 0;
	}

	switch(ch) {
		case '\n':
			set_cursor(offset + (max_cols - (offset % max_cols)));
			break;
		default:
			set_char(offset, ch);
			set_cursor(offset + 1);
			break;
	}
}

void vga_puts(char *string) {
	for(int i = 0; string[i] != '\0'; i++) {
		vga_putc(string[i]);
	}
}

void vga_init(uint32_t width, uint32_t height) {
	max_rows = height;
	max_cols = width;
}
