#include <kernel/debug_output.h>
#include <kernel/page_alloc.h>
#include <kernel/arch/memlayout.h>
#include <kernel/arch/mmu.h>

#include <stdint.h>
#include <stddef.h>

page_alloc_allocator_t phys_alloc;

void setup_phys_alloc(void* alloc_pool, size_t alloc_pool_size) {
     kprintf("Allocating memory from pool at 0x%p\n", alloc_pool);
     page_alloc_init(&phys_alloc, false, NULL, NULL, NULL);
     page_alloc_init_region(&phys_alloc,alloc_pool,alloc_pool_size);
     kprintf("Got %i free pages of memory\n", page_alloc_count_free_pages(&phys_alloc));
}

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)

mmu_page_directory_t kernel_page_dir;
mmu_page_table_t     kimage_page_table;

void load_pdbr(uintptr_t addr) {
     __asm__("mov %0, %%cr3" : : "r"(addr));
}

void * get_physaddr(void * virtualaddr)
{
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
 
    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}

void setup_paging() {
     // create a blank page directory
     clear_page_directory(&kernel_page_dir);

     // create a blank page table
     clear_page_table(&kimage_page_table);

     // create kernel image mapping in a page table
     int i=0;
     for(i=0; i<1024; i++) {
         kimage_page_table[i] = (i * 0x1000) |MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_CPU_GLOBAL;
     }

     // put the kimage into the page directory
     kernel_page_dir[PAGE_DIRECTORY_INDEX(KERN_BASE)] = (uint32_t)(EARLY_V2P(&kimage_page_table)) | 3;

     // switch to the new page directory
     load_pdbr(EARLY_V2P(&kernel_page_dir));

     kprintf("physical address of kimage_page_table: 0x%08p\n", get_physaddr(&kimage_page_table));
     kprintf("virtual address of kimage_page_table:  0x%08p\n", &kimage_page_table);
}

void kmain(void* alloc_pool, size_t alloc_pool_size) {
     kprintf("AMIX starting....\n\n");
     setup_phys_alloc(alloc_pool, alloc_pool_size);
     setup_paging();
     for(;;);
}
