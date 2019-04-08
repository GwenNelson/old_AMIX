#pragma once

#define SYS_debug_out 0x1

#define SYSCALL_COUNT 1
uint32_t sys_debug_out(char c);

extern uint32_t (*syscalls_table[SYSCALL_COUNT+1])(uint32_t,uint32_t,uint32_t,uint32_t);
