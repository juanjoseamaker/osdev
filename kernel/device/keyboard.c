#include "../int/isr.h"
#include "../kernel.h"
#include "port.h"
#include "keyboard.h"

#include <stdbool.h>

#define KM_MAX 57

static bool caps_lock = false;

const char keymap_us[] = {
  '?', '?', '1', '2', '3', '4', '5',
  '6', '7', '8', '9', '0', '-', '=',
  '\0', '?', 'q', 'w', 'e', 'r', 't',
  'y', 'u', 'i', 'o', 'p', '[', ']',
  '\0', '?', 'a', 's', 'd', 'f', 'g',
  'h', 'j', 'k', 'l', ';', '\'', '`',
  '?', '\\', 'z', 'x', 'c', 'v', 'b',
  'n', 'm', ',', '.', '/', '?', '?',
  '?', ' '
};

char scancode_to_char(uint8_t scancode) {
    char ch = keymap_us[(int) scancode];

    if(ch >= 'a' && ch <= 'z' && caps_lock) {
        return ch - 32;
    }

    return ch;
}

static void keyboard_callback(registers_t *regs) {
    uint8_t scancode = port_byte_in(0x60);

    if(scancode == 0x3a) {
        caps_lock = !caps_lock;
    } else if(scancode == 0x0e) {
        // TODO: backspace
    } else if(scancode == 0x1c) {
        printk("\n");
    } else {
        if(scancode > KM_MAX) return;

        char ch = scancode_to_char(scancode);
        char str[2] = {ch, '\0'};

        printk(str);
    }
}

void init_keyboard() {
    register_interrupt_handler(IRQ1, keyboard_callback);
}
