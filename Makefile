

vasm:
	vasm -Fbin -c02 -dotdir -Iinclude -o bios.bin main.asm

hex:
	hexdump -C bios.bin

flash: vasm
	minipro -p AT28C256 -w bios.bin
