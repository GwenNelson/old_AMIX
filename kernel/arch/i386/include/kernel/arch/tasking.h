#pragma once

#include <stdint.h>
#include <stdbool.h>

bool tasking_ready;

typedef struct task_regs_t;
typedef struct task_regs_t {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} task_regs_t;

typedef struct task_control_block_t task_control_block_t;
typedef struct task_control_block_t {
	task_control_block_t* next;
	task_regs_t regs;
} task_control_block_t;

void init_tasking();
void create_task(task_control_block_t* task, void* entry, uint32_t flags, uint32_t* pdir);

void yield();
void switch_to_task(task_regs_t *old_regs, task_regs_t* new_regs);
