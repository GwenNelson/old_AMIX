.PHONY: all run-qemu clean

all: kernel
	make -C kernel install

clean: kernel
	make -C kernel clean

run-qemu: all
	qemu-system-i386 -kernel sysroot/boot/kernel.bin -serial mon:stdio -m 4G 
