#pragma once

#pragma once

uint32_t sys_debug_out(char c);

#define X(num,name)
	#include <kernel/syscalls.def>
#undef X
extern uint32_t (*syscalls_table[SYSCALL_COUNT+1])(uint32_t,uint32_t,uint32_t,uint32_t);
