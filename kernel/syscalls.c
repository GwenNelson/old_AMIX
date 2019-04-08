#include <stdint.h>
#include <stddef.h>
#include <kernel/syscalls.h>

uint32_t sys_debug_out(char c) {
	kprintf("%c",c);
}

uint32_t (*syscalls_table[SYSCALL_COUNT+1])(uint32_t,uint32_t,uint32_t,uint32_t) = {
	[SYS_debug_out]	&sys_debug_out,
};
