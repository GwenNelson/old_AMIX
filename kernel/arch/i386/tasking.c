#include <stddef.h>
#include <stdint.h>
#include <kernel/arch/tasking.h>
#include <kernel/debug_output.h>
#include <kernel/arch/portio.h>
#include <kernel/kalloc.h>
#include <kernel/arch/mmu.h>
#include <kernel/arch/memlayout.h>
#include <kernel/kmain.h>

task_control_block_t* running_task;
task_control_block_t main_task;

void init_tasking() {
     main_task.next  = NULL;
     running_task = &main_task;

     tasking_ready = true;
}

void create_task(task_control_block_t *task, void* entry, uint32_t flags, uint32_t *pagedir) {
    	task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (uint32_t) entry;
    task->regs.cr3 = (uint32_t) pagedir;
    task->regs.esp = (uint32_t) kalloc()+4096;
    task->next = 0;
}

void add_task(task_control_block_t* task) {
     asm volatile("cli");
      task_control_block_t* t=running_task;
      while((t->next != NULL)) t=t->next;
      t->next = task;
     asm volatile("sti");
}

void yield() {
     if(!tasking_ready) return;
     asm volatile("cli");
     task_control_block_t *last = running_task;
     running_task = running_task->next;
     if(running_task==NULL) running_task=&main_task;
     asm volatile("sti");
     switch_to_task(&last->regs, &running_task->regs);
}

