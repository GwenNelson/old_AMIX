#include <stdint.h>
#include <stddef.h>
#include <kernel/arch/idt.h>
#include <kernel/arch/tasking.h>
#include <kernel/arch/mmu.h>
#include <kernel/arch/memlayout.h>
#include <kernel/kalloc.h>
#include <kernel/syscalls.h>

int sys_debug_out(char c) {
    kprintf("%c",c);
    return 0;
}

int sys_debug_out_num(uintptr_t n) {
    kprintf("0x%08x",n);
}

uint32_t sys_get_tid() {
    uint32_t tid = running_task->tid;
    return tid;
}

extern char* usercode;

uintptr_t (*syscalls_table[SYSCALL_COUNT+1])(uintptr_t,uintptr_t,uintptr_t,uintptr_t) = {
#define X(num,name) [num] &sys_##name,
	#include <kernel/syscalls.def>
#undef X
};

ISR(syscall_i80_handler) {
	uint32_t* userstack  = frame->useresp;
	uint32_t syscall_no  = userstack[0];

	if(syscall_no >0 && syscall_no < (sizeof(syscalls_table)/sizeof(void*)) && syscalls_table[syscall_no]) {
		running_task->regs.eip = __builtin_extract_return_addr(__builtin_return_address(0));
		userstack[0] = syscalls_table[syscall_no](userstack[1],userstack[2],userstack[3],userstack[4]);
	} else {
		dump_frame(frame);
		kprintf("unknown syscall number 0x%x\n",syscall_no);
	}

}
