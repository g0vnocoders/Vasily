CC=i686-elf-gcc
AS=i686-elf-gcc
LD=i686-elf-gcc

x86_asm=$(wildcard asm/x86-32/*.S)
x86_o=$(x86_asm:.S=.o) 

c_src= $(wildcard src/*.c) $(wildcard src/dev/*.c) $(wildcard src/fs/*.c)
c_obj=$(c_src:.c=.o)
LDFLAGS=-ffreestanding -nostdlib -lgcc 
CFLAGS= -Wall -Wextra -ffreestanding -Ofast -Wall -Wextra -I/usr/include/multiboot -I./include -std=gnu99 -pipe -march=i386 -mtune=i386 
ASFLAGS=-I/usr/include/multiboot
all: vasily.iso $(c_src) $(x86_asm)

clean:
	-rm src/*.o src/fs/*.o src/dev/*.o asm/x86-32/*.o
	

test: all
	qemu-system-i386 -cdrom vasily.iso -hda test.img -hdb test.img2 -boot d
testahci: all
	qemu-system-i386 -cdrom vasily.iso -drive id=disk,file=test.img,if=none,format=raw -device ahci,id=ahci -device ide-hd,drive=disk,bus=ahci.0 -device ide-hd,drive=disk2,bus=ahci.1 -boot d -drive    id=disk2,file=test.img2,if=none,format=raw 
test4g: all
		qemu-system-i386 -cdrom vasily.iso -hda test.img -hdb test.img2 -boot d -m 4096M
bochs_test: all
	-rm *.lock
	bochs -f conf -q
install: vmvasily 
	sudo cp vmvasily /boot
vasily.iso:  vmvasily
	cp vmvasily iso/boot/vmvasily
	grub-mkrescue -o vasily.iso iso

vmvasily: $(x86_o) $(c_obj) link.ld 
	$(LD) -Wl,-Tlink.ld,-Map=ld.map -o $@ $(x86_o) $(LDFLAGS) $(c_obj)
