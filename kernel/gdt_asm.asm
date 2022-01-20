[bits 32]

section .data

gdtr dw 0
	 dd 0

section .text

global set_gdt

set_gdt:
	mov eax, [esp + 4]
	mov [gdtr + 2], eax
	mov ax, [esp + 8]
	mov [gdtr], ax
	lgdt [gdtr]
	ret

global reload_segments

reload_segments:
	jmp 0x08:reload_CS
reload_CS:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
