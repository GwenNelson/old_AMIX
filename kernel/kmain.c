#include <kernel/debug_output.h>
#include <kernel/arch/memlayout.h>
#include <kernel/arch/mmu.h>
#include <kernel/arch/idt.h>
#include <kernel/arch/tasking.h>
#include <kernel/arch/portio.h>
#include <kernel/kalloc.h>
#include <kernel/printf.h>
#include <kernel/timer.h>
#include <stdint.h>
#include <stddef.h>

void setup_phys_alloc(void* alloc_pool, size_t alloc_pool_size) {
     kprintf("Allocating memory from pool at 0x%08p\n", alloc_pool);
     freerange(alloc_pool, alloc_pool+alloc_pool_size);     
}

mmu_page_directory_t kernel_page_dir;

void setup_paging() {
     // create a blank page directory
     clear_page_directory(&kernel_page_dir);

    // create low mapping in the page table
     uintptr_t i=0;
     for(i=0; i<1024; i++) {
	mmu_map_page(&kernel_page_dir,(void*)(i * 0x1000), (void*)(i * 0x1000) + KERN_BASE,MMU_PTE_WRITABLE|MMU_PTE_PRESENT);
     }



     // first map the text section of the kernel
     extern char* code;
     extern char* ecode;
     size_t code_len = ecode-code;
     i=&code;
     for(i=&code; i< &ecode; i += 4096) {
         mmu_map_page(&kernel_page_dir,(void*)(i-KERN_BASE), (void*)(i), MMU_PTE_PRESENT);
     }

     // now the data
     extern char* data;
     extern char* _edata;
     size_t data_len = _edata-data;
     for(i=&data; i<&_edata; i += 4096) {
         mmu_map_page(&kernel_page_dir,(void*)(i-KERN_BASE), (void*)(i), MMU_PTE_WRITABLE|MMU_PTE_PRESENT);
     }

      mmu_map_page(&kernel_page_dir,0xb8000,0xC03FF000,MMU_PTE_USER|MMU_PTE_WRITABLE|MMU_PTE_PRESENT);
     // switch to the new page directory
     load_page_directory((mmu_page_directory_t*)  EARLY_V2P(&kernel_page_dir));
}

void timer_cb() {
     if(tasking_ready) { 
	     yield();
     }
}

void setup_timer(timer_t* timer) {
     timer->callback = &timer_cb;
}

char static_pool[4096*512] __attribute((aligned(4096))); // just enough to bootstrap

mmu_page_directory_t user_proc_dir;
task_control_block_t first_user_proc;

mmu_page_directory_t second_user_proc_dir;
task_control_block_t second_user_proc;

extern char* usercode;
extern char* usercode_end;

void setup_usercode(mmu_page_directory_t* pd, task_control_block_t* tcb) {
//     clear_page_directory(&user_proc_dir);
     // copy the kernel mappings
     __builtin_memcpy(pd,&kernel_page_dir,4096);
     memset((void*)tcb,0,sizeof(tcb));

     // allocate a single physical page and copy usercode into it
     uint32_t usercode_len = (uint32_t)usercode_end - (uint32_t)usercode;
     char* p = (char*)kalloc();
     __builtin_memcpy(p,usercode,4096);

     // map the code page where usercode expects to start
     mmu_map_page(pd,V2P(p),0x00200000,MMU_PTE_PRESENT|MMU_PTE_USER);

     // map the page where usercode expects stack
     mmu_map_page(pd,EARLY_V2P(kalloc()),0x00201000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);
     mmu_map_page(pd,EARLY_V2P(kalloc()),0x00202000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

     // setup the TCB
     create_task(tcb,0x00200000,DEFAULT_TASK_FLAGS,V2P(&user_proc_dir));
     first_user_proc.regs.esp = 0x00202000;

     // finally, run the bugger
     add_task(tcb);
}

void kmain(void* alloc_pool, size_t alloc_pool_size, timer_t* timer) {
     kprintf("AMIX starting....\n\n");

     // TODO: fix this, use an entry-based system or something
     setup_phys_alloc(static_pool, 4096*512);
     setup_paging();
     setup_phys_alloc(alloc_pool+KERN_BASE, alloc_pool_size);

     init_tasking();

     setup_usercode(&user_proc_dir, &first_user_proc);

     setup_timer(timer);


     int i=0;
     for(i=0; i<60; i++) {
		setup_usercode(kalloc(), kalloc());
     }
     asm volatile("sti");


     // idle loop
     for(;;) yield(); 
}
