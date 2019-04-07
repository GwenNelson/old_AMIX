#include <kernel/arch/memlayout.h>
#include <kernel/arch/mmu.h>
#include <kernel/kalloc.h>
#include <kernel/debug_output.h>

void clear_page_directory(mmu_page_directory_t* dir) {
     // first we clear out the whole directory
     int i=0;
     for(i=0; i<1024; i++) {
         (*dir)[i] = MMU_PDE_WRITABLE;
     }
     // then we set the highest entry to the directory itself, allowing access to modify the page table later
     (*dir)[1023] = (uint32_t)(EARLY_V2P(dir)) | MMU_PDE_PRESENT|MMU_PDE_WRITABLE;
}

void clear_page_table(mmu_page_table_t* table) {
     // we just clear out the whole table
     int i=0;
     for(i=0; i<1024; i++) {
         (*table)[i] = MMU_PTE_WRITABLE;
     }
}

void load_page_directory(mmu_page_directory_t* phys_addr) {
     uintptr_t addr = (uintptr_t)phys_addr;
     __asm__("mov %0, %%cr3" : : "r"(addr));
}

void mmu_map_page(mmu_page_directory_t* dir, void* phys_addr, void* virt_addr, uint32_t flags) {

	// get page table
     mmu_pde_t* e = &((*dir)[PAGE_DIRECTORY_INDEX((uintptr_t)virt_addr)]);
     mmu_page_table_t* table;
     if( (*e & MMU_PTE_PRESENT) != MMU_PTE_PRESENT) {
        kprintf("Need to allocate new page for mapping %08p -> %08p\n",phys_addr,virt_addr);
        void* new_page = kalloc();
        kprintf("Allocated at 0x%08p\n", new_page);
	clear_page_table((mmu_page_table_t*)new_page);
	table = (mmu_page_table_t*)new_page;
        *e = (((uintptr_t)EARLY_V2P(table)))|MMU_PDE_PRESENT|MMU_PDE_WRITABLE;
     }
     (*table)[PAGE_TABLE_INDEX((uint32_t)virt_addr)] = (((uint32_t)phys_addr))|flags;
}

// shamelessly ripped from the osdev wiki
void* V2P(void* virtualaddr) {
   unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;
 
    unsigned long * pd = (unsigned long *)0xFFFFF000;
    // Here you need to check whether the PD entry is present.
 
    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
 
    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));

}
