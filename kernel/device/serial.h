#pragma once

#include <stdint.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

void serial_enable(uint16_t device);
void serial_send(uint16_t device, char out);
char serial_recv(uint16_t device);
char serial_recv_async(uint16_t device);

void serial_terminal_putc(char ch);