#pragma once

int sys_debug_out(char c);
int sys_debug_out_num(uintptr_t n);
uint32_t sys_get_tid();

#define SYSCALL_COUNT 4

extern uintptr_t (*syscalls_table[SYSCALL_COUNT+1])(uintptr_t,uintptr_t,uintptr_t,uintptr_t);
