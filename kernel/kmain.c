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

     // create kernel image mapping in the page table
     int i=0;
     for(i=0; i<1024; i++) {
	mmu_map_page(&kernel_page_dir,(void*)(i * 0x1000), (void*)(i * 0x1000) + KERN_BASE,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_CPU_GLOBAL);
     }

     mmu_map_page(&kernel_page_dir,0xb8000,0xC03FF000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT);
     // switch to the new page directory
     load_page_directory((mmu_page_directory_t*)  EARLY_V2P(&kernel_page_dir));

}

void timer_cb() {
     if(tasking_ready) { 
	     console_put('.');
	     yield();
     }
}

void setup_timer(timer_t* timer) {
     timer->callback = &timer_cb;
}

char static_pool[4096*256] __attribute((aligned(4096)));

void a() {for(;;) kprintf("A");}
void b() {for(;;) kprintf("B");}

task_control_block_t task_a;
task_control_block_t task_b;

void kmain(void* alloc_pool, size_t alloc_pool_size, timer_t* timer) {
     kprintf("AMIX starting....\n\n");
     setup_phys_alloc(static_pool, 4096*256);
     setup_phys_alloc(alloc_pool+KERN_BASE, alloc_pool_size);
     setup_paging();
     
     setup_timer(timer);

     init_tasking();

     create_task(&task_a,a, DEFAULT_TASK_FLAGS, V2P(&kernel_page_dir));
     create_task(&task_b,b, DEFAULT_TASK_FLAGS, V2P(&kernel_page_dir));

     add_task(&task_a);
     add_task(&task_b);

     asm volatile("sti");


     // idle loop
     for(;;) asm volatile("hlt");
}
