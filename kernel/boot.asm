[bits 32]

; multiboot
MAGIC equ 0x1BADB002
FLAGS equ 1<<0 | 1<<1 | 1<<2 ; 4K page align, memory info, graphics fields
CHECKSUM equ -(MAGIC + FLAGS)
; MODE_TYPE equ 1 ; EGA-standard text mode
MODE_TYPE equ 0 ; linear graphics mode
WIDTH equ 80
HEIGHT equ 24
DEPTH equ 0 ; zero in a text mode

; multiboot header
section .multiboot

align 4

dd MAGIC
dd FLAGS
dd CHECKSUM
dd 0
dd 0
dd 0
dd 0
dd 0
dd MODE_TYPE
dd WIDTH
dd HEIGHT
dd DEPTH

; stack
section .bootstrap_stack

stack_bottom:
times 32768 db 0 ; 16 KiB
stack_top:

section .text

[extern main]
global _start

_start:
	mov esp, stack_top

	; push the pointer to the multiboot information structure
	push ebx

	; push the magic value
	push eax

	cli
	
	call main
	
	cli
	
hlt_loop:
	hlt
	jmp hlt_loop
	
