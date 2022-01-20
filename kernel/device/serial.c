#include "serial.h"
#include "port.h"

#include <stdint.h>

void serial_enable(uint16_t device) {
    port_byte_out(device + 1, 0x00);
	port_byte_out(device + 3, 0x80); /* Enable divisor mode */
	port_byte_out(device + 0, 0x03); /* Div Low:  03 Set the port to 38400 bps */
	port_byte_out(device + 1, 0x00); /* Div High: 00 */
	port_byte_out(device + 3, 0x03);
	port_byte_out(device + 2, 0xC7);
	port_byte_out(device + 4, 0x0B);
}


int serial_rcvd(uint16_t device) {
	return port_byte_in(device + 5) & 1;
}

char serial_recv(uint16_t device) {
	while(serial_rcvd(device) == 0) ;
	return port_byte_in(device);
}

char serial_recv_async(uint16_t device) {
	return port_byte_in(device);
}

char serial_transmit_empty(uint16_t device) {
	return port_byte_in(device + 5) & 0x20;
}

void serial_send(uint16_t device, char out) {
	while(serial_transmit_empty(device) == 0);
	port_byte_out(device, out);
}

void serial_terminal_putc(char ch) {
    serial_send(COM1, ch);
}