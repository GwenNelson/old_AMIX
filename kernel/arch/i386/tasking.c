#include <stddef.h>
#include <stdint.h>
#include <kernel/arch/tasking.h>
#include <kernel/debug_output.h>
#include <kernel/arch/portio.h>
#include <kernel/kalloc.h>

task_control_block_t* running_task;
task_control_block_t main_task;
task_control_block_t other_task;

void other_task_main() {

        for(;;) {
		kprintf("B  "); 
	}
}


void init_tasking() {
     asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(main_task.regs.cr3)::"%eax");
     asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(main_task.regs.eflags)::"%eax");

     create_task(&other_task,other_task_main, main_task.regs.eflags, main_task.regs.cr3);
     main_task.next = &other_task;
     other_task.next = &main_task;
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

void yield() {
     if(!tasking_ready) return;
     asm volatile("cli");
     task_control_block_t *last = running_task;
     running_task = running_task->next;
     asm volatile("sti");
     switch_to_task(&last->regs, &running_task->regs);
}

