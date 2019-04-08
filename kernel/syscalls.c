#include <stdint.h>
#include <stddef.h>
#include <kernel/arch/idt.h>
#include <kernel/syscalls.h>

uint32_t sys_debug_out(char c) {
	kprintf("%c",c);
}

uint32_t (*syscalls_table[SYSCALL_COUNT+1])(uint32_t,uint32_t,uint32_t,uint32_t) = {
#define X(num,name) [num] &sys_##name,
	#include <kernel/syscalls.def>
#undef X
};

ISR(syscall_i80_handler) {
	uint32_t* userstack  = frame->useresp;
	uint32_t syscall_no  = userstack[0];
	if(syscall_no >0 && syscall_no < (sizeof(syscalls_table)/sizeof(void*)) && syscalls_table[syscall_no]) {
		userstack[0] = syscalls_table[syscall_no](userstack[1],userstack[2],userstack[3],userstack[4]);
	} else {
		kprintf("unknown syscall number 0x%x\n",syscall_no);
	}

}
