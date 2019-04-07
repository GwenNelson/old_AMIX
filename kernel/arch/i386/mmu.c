#include <kernel/arch/memlayout.h>
#include <kernel/arch/mmu.h>

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
