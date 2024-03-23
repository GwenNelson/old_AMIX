#include <kernel/arch/idt.h>
#include <kernel/arch/portio.h>




ISR(kbd_handler) {
	kprintf(".");
	char scan = inb(0x60);
	outb(0x20,0x20); outb(0xa0,0x20);
}
