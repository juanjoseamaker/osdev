#include "kernel.h"

__attribute__((noreturn)) void panic(char *error) {
    printk("\nPANIC: %s\n", error);

    asm volatile("cli");
    for(;;);

    __builtin_unreachable();
}