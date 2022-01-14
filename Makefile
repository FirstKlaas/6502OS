

vasm:
	vasm -Fbin -c02 -dotdir -Iinclude -o bios.bin main.asm

hex:
	hexdump -C bios.bin