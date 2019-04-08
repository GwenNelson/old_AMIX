#include <kernel/memset.h>
#include <kernel/arch/memlayout.h>
#include <kernel/arch/idt.h>
#include <kernel/arch/portio.h>

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void idt_set_gate(uint8_t num, uintptr_t base, uint16_t sel, uint8_t flags) {
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[num].sel     = sel;
	idt_entries[num].always0 = 0;
	idt_entries[num].flags   = flags|0x60;
}

void dump_frame(interrupt_frame_t* frame) {
     kprintf("eip=0x%08x, cs=0x%08x, eflags=0x%08x, useresp=0x%08x, ss=0x%08x\n",
		     frame->eip, frame->cs, frame->eflags, frame->useresp, frame->ss);
}

ISR(default_handler) {
	kprintf("DEFAULT HANDLER\n");
	dump_frame(frame);
}

ISR(div_zero_handler) {
   	kprintf("Divide by zero in 0x%08p\n",frame->eip);
	// TODO - make this send signals to tasks and stuff
}

ISR(debug_handler) {
	kprintf("Debug interrupt triggered\n");
	dump_frame(frame);
}

ISR(nmi_handler) {
	kprintf("NMI occurred! Immediate halt\n");
	kprintf("This is usually a sign of hardware failure - backup your data\n");
	for(;;) asm volatile("cli; hlt");
}

ISR(breakpoint_handler) {
	kprintf("Debug breakpoint triggered\n");
	dump_frame(frame);
	for(;;);
}

ISR(invalid_opcode_handler) {
	// TODO - emulation etc
	kprintf("Invalid opcode encountered at 0x%08p\n", frame->eip);
	dump_frame(frame);
}

ISR_FAULT(gpf_handler) {
	kprintf("General protection fault at 0x%08p\n", frame->eip);
	dump_frame(frame);
	for(;;) asm volatile("cli; hlt");
}

ISR_FAULT(page_fault) {
	kprintf("PAGE FAULT!\n");
	// TODO - pass this to the VMM
}

ISR_FAULT(double_fault_handler) {
	kprintf("Double fault\n");
	dump_frame(frame);
	for(;;) asm volatile("cli; hlt");	
}

ISR(timer_handler);

/*ISR(timer_handler) {
	kprintf(".");
	outb(0x20,0x20);
}*/

static inline void lidt(void* base, uint16_t size)
{   // This function works in 32 and 64bit mode
    struct {
        uint16_t length;
        void*    base;
    } __attribute__((packed)) IDTR = { size, base };
 
    asm ( "lidt %0" : : "m"(IDTR) );  // let the compiler choose an addressing mode
}

void init_idt() {
     idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
     idt_ptr.base  = (uint32_t)&idt_entries;
     memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

     int i=0;
     for(i=0; i<256; i++) idt_set_gate(i, &default_handler, 0x8, 0x8F);

     // TODO - add abstraction for routing IRQs to callbacks in main kernel

     idt_set_gate(0x00, &div_zero_handler,      0x8,0x8F);
     idt_set_gate(0x01, &debug_handler,         0x8,0x8F);
     idt_set_gate(0x02, &nmi_handler,           0x8,0x8F);
     idt_set_gate(0x03, &breakpoint_handler,    0x8,0x8F);
     idt_set_gate(0x04, &invalid_opcode_handler,0x8,0x8F);
     idt_set_gate(0x08, &double_fault_handler,  0x8,0x8F);
     idt_set_gate(0x0D, &gpf_handler,           0x8,0x8F);
     idt_set_gate(0x20, &timer_handler,		0x8,0x8F);
     lidt(idt_ptr.base, idt_ptr.limit);
}
