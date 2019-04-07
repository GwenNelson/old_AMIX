#include <kernel/debug_output.h>
#include <kernel/arch/memlayout.h>
#include <kernel/arch/mmu.h>
#include <kernel/kalloc.h>
#include <kernel/printf.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void setup_phys_alloc(void* alloc_pool, size_t alloc_pool_size) {
     kprintf("Allocating memory from pool at 0x%08p\n", alloc_pool);
     freerange(alloc_pool, alloc_pool+alloc_pool_size);     
     kprintf("Physical page allocation ready\n");
}

mmu_page_directory_t kernel_page_dir;
mmu_page_table_t     kimage_page_table;

void setup_paging() {
     // create a blank page directory
     clear_page_directory(&kernel_page_dir);

     // create a blank page table
     clear_page_table(&kimage_page_table);

     // create kernel image mapping in the page table
     int i=0;
     for(i=0; i<1024; i++) {
	mmu_map_page(&kernel_page_dir,(void*)(i * 0x1000), (void*)(i * 0x1000) + KERN_BASE,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_CPU_GLOBAL);
     }

     // switch to the new page directory
     load_page_directory(EARLY_V2P(&kernel_page_dir));

}

char static_pool[4096*256] __attribute((aligned(4096)));


void kmain(void* alloc_pool, size_t alloc_pool_size) {
     kprintf("AMIX starting....\n\n");
     setup_phys_alloc(static_pool, 4096*256);
     setup_phys_alloc(alloc_pool+KERN_BASE, alloc_pool_size);
     setup_paging();


     void* test = kalloc();
     snprintf(test,4096,"%s","Hello world"); 
     mmu_map_page(&kernel_page_dir,V2P(test),0xBEEF0000,MMU_PTE_PRESENT|MMU_PTE_WRITABLE);
     kprintf("%s",0xBEEF0000);
     for(;;);
}
