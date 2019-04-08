#include <stddef.h>
#include <stdint.h>
#include <kernel/arch/tasking.h>
#include <kernel/debug_output.h>
#include <kernel/arch/portio.h>
#include <kernel/arch/gdt.h>
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
    task->start_stack = (uint32_t)kalloc();
    task->regs.esp = task->start_stack+4096;

    task->kernel_stack = (uint32_t) kalloc()+4096;
    // stupid hack to get a reasonably unique tid
    task->tid = (uint32_t)task;

    mmu_map_page(&pagedir,V2P(task->regs.esp-4096),    task->regs.esp-4096,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);
    mmu_map_page(&pagedir,V2P(task->kernel_stack-4096),task->kernel_stack-4096,MMU_PTE_WRITABLE|MMU_PTE_PRESENT);

    task->next = 0;
}

void add_task(task_control_block_t* task) {
     asm volatile("cli");
      task_control_block_t* t=running_task;
      while((t->next != NULL)) t=t->next;
      t->next = task;
     asm volatile("sti");
     yield();
}

void yield() {
     if(!tasking_ready) return;
     asm volatile("cli");
     task_control_block_t *last = running_task;
     running_task = running_task->next;
     if(running_task==NULL) running_task=&main_task;
     load_page_directory(running_task->regs.cr3);
     set_kernel_stack(running_task->kernel_stack);
     asm volatile("sti");
     switch_to_task(&last->regs, &running_task->regs);
}

