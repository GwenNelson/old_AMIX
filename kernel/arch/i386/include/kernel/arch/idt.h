#pragma once

#include <stdint.h>
#include <stddef.h>

struct idt_entry_struct
{
   uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16_t sel;                 // Kernel segment selector.
   uint8_t  always0;             // This must always be zero.
   uint8_t  flags;               // More flags. See documentation.
   uint16_t base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
   uint16_t limit;
   uint32_t base;                // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

void idt_set_gate(uint8_t num, uintptr_t base, uint16_t sel, uint8_t flags);
void init_idt();

typedef struct interrupt_frame_t {
	void* eip;
	uint16_t cs, :16;
	uint32_t eflags;
	void* esp;
	uint16_t ss, :16;
} interrupt_frame_t;
