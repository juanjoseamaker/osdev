#include "../kernel.h"
#include "mouse.h"
#include "port.h"
#include "../int/isr.h"

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_WRITE  0xD4

uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];
char buttons = 0;
int mouse_x = 0;
int mouse_y = 0;

char mouse_buttons() {
    return buttons;
}

int mouse_coord_x() {
	return mouse_x;
}

int mouse_coord_y() {
	return mouse_y;
}

void mouse_wait_to_output() {
	uint32_t timeout = 100000;
    while (--timeout) {
        if (!((port_byte_in(MOUSE_STATUS) & 0x02))) {
			return;
		}
	}

	printk("DEBUG: Mouse timeout\n");
}

void mouse_wait_to_read() {
    uint32_t timeout = 100000;
    while (--timeout) {
		if ((port_byte_in(MOUSE_STATUS) & 0x01) == 1) {
			return;
		}
	}

	printk("DEBUG: Mouse timeout\n");
}

void mouse_write(uint8_t write) {
	mouse_wait_to_output();
	port_byte_out(MOUSE_STATUS, MOUSE_WRITE);
	mouse_wait_to_output();
	port_byte_out(MOUSE_PORT, write);
}

uint8_t mouse_read() {
	mouse_wait_to_read();
	return port_byte_in(MOUSE_PORT);
}

static void mouse_callback(registers_t *regs) {
    uint8_t status = port_byte_in(MOUSE_STATUS);
	while (status & 0x01) {
		int8_t mouse_in = port_byte_in(MOUSE_PORT);
		if (status & 0x20) {
			switch (mouse_cycle) {
				case 0:
					mouse_byte[0] = mouse_in;
					if (!(mouse_in & 0x08)) return;
					++mouse_cycle;
					break;
				case 1:
					mouse_byte[1] = mouse_in;
					++mouse_cycle;
					break;
				case 2:
					mouse_byte[2] = mouse_in;
					if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40) {
						break;
					}

					if (mouse_byte[0] & 0x01)
						buttons |= LEFT_BUTTON;
					else
						buttons &= ~LEFT_BUTTON;
                    if (mouse_byte[0] & 0x02)
						buttons |= RIGHT_BUTTON;
					else
						buttons &= ~RIGHT_BUTTON;						

	                mouse_x += mouse_byte[1];

					mouse_y -= mouse_byte[2];

					if (mouse_x < 0)
						mouse_x = 0;

					if (mouse_y < 0)
						mouse_y = 0;
						
					mouse_cycle = 0;
			}
		}

		status = port_byte_in(MOUSE_STATUS);
	}
}

void init_mouse() {
	uint8_t status;
	mouse_wait_to_output();
	port_byte_out(MOUSE_STATUS, 0xA8);
	mouse_wait_to_output();
	port_byte_out(MOUSE_STATUS, 0x20);
	mouse_wait_to_read();
	status = port_byte_in(0x60) | 2;
	mouse_wait_to_output();
	port_byte_out(MOUSE_STATUS, 0x60);
	mouse_wait_to_output();
	port_byte_out(MOUSE_PORT, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();

	register_interrupt_handler(IRQ12, mouse_callback);
}
