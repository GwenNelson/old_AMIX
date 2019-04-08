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
	     yield();
     }
}

void setup_timer(timer_t* timer) {
     timer->callback = &timer_cb;
}

char static_pool[4096*128] __attribute((aligned(4096)));

mmu_page_directory_t user_proc_dir;
task_control_block_t first_user_proc;

extern char* usercode;
extern char* usercode_end;

void setup_user() {
     clear_page_directory(&user_proc_dir);

     // setup the kernel mappings in user space
     int i=0;
     for(i=0; i<1024; i++) {
	mmu_map_page(&user_proc_dir,(void*)(i * 0x1000), (void*)(i * 0x1000) + KERN_BASE,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_CPU_GLOBAL|MMU_PTE_USER);
     }
     mmu_map_page(&user_proc_dir,0xb8000,0xC03FF000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

     // allocate a single physical page and copy usercode into it
     uint32_t usercode_len = (uint32_t)&usercode_end - (uint32_t)&usercode;
     kprintf("Usercode is at 0x%08x and %d bytes long\n", (uint32_t)&usercode, usercode_len);
     char* p = (char*)kalloc();
     __builtin_memcpy(p,&usercode,4096);

     // map the page where usercode expects to start
     mmu_map_page(&user_proc_dir,V2P(p),0x00200000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

     // setup the TCB
     create_task(&first_user_proc,0x00200000,DEFAULT_TASK_FLAGS,V2P(&user_proc_dir));

     kprintf("About to start first user process with TID 0x%08x\n",first_user_proc.tid);
     // finally, run the bugger
     add_task(&first_user_proc);
}

void kmain(void* alloc_pool, size_t alloc_pool_size, timer_t* timer) {
     kprintf("AMIX starting....\n\n");
     setup_phys_alloc(static_pool, 4096*128);
     setup_phys_alloc(alloc_pool+KERN_BASE, alloc_pool_size);
     setup_paging();

     asm volatile("sti");

     setup_timer(timer);

     init_tasking();


     setup_user();

     // idle loop
     for(;;) asm volatile("hlt");
}
