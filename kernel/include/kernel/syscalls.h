#pragma once

int sys_debug_out(char c);

#define X(num,name)
	#include <kernel/syscalls.def>
#undef X
extern uintptr_t (*syscalls_table[SYSCALL_COUNT+1])(uintptr_t,uintptr_t,uintptr_t,uintptr_t);
