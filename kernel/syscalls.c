#include <stdint.h>
#include <stddef.h>
#include <kernel/arch/idt.h>
#include <kernel/syscalls.h>

int sys_debug_out(char c) {
    kprintf("%c",c);
    return 0;
}

uintptr_t (*syscalls_table[SYSCALL_COUNT+1])(uintptr_t,uintptr_t,uintptr_t,uintptr_t) = {
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
