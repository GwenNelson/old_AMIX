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
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t useresp;
	uint32_t ss;
} interrupt_frame_t;

void dump_frame(interrupt_frame_t* frame);

#define ISR(name) __attribute__((interrupt)) void name (interrupt_frame_t* frame)
#define ISR_FAULT(name)  __attribute__((interrupt)) void name (interrupt_frame_t* frame, uint32_t errno)
