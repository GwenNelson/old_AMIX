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
task_control_block_t other_task;

void other_task_main() {

        for(;;) {
		kprintf("B  "); 
	}
}

#define DEFAULT_FLAGS 0x0002|0x0200

void init_tasking() {

     create_task(&other_task,other_task_main, DEFAULT_FLAGS, V2P(&kernel_page_dir));
     main_task.next = &other_task;
     other_task.next = &main_task;
     running_task = &main_task;
     tasking_ready = true;
}

void create_task(task_control_block_t *task, void* entry, uint32_t flags, uint32_t *pagedir) {
     kprintf("EFLAGS=%08x\n",flags);
    	task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = DEFAULT_FLAGS;
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

