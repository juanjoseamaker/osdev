#pragma once

#include "device/vga.h"
#include "util/format.h"
#include "device/serial.h"
#include "config.h"

#ifdef SERIAL_TTY
#define printk(...) format(serial_terminal_putc,__VA_ARGS__)
#endif

#ifdef VGA_TTY
#define printk(...) format(vga_putc,__VA_ARGS__)
#endif

__attribute__((noreturn)) void panic(char *error);

typedef struct {
    // data segment selector
    uint32_t ds;
    // general purpose registers pushed by pusha
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    // pushed by isr procedure
    uint32_t int_no, err_code;
    // pushed by CPU automatically
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;
