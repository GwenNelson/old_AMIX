#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGESIZE 4096

enum MMU_PDE_FLAGS {
	MMU_PDE_EMPTY        = 0x000,
	MMU_PDE_PRESENT      = 0x001,
	MMU_PDE_WRITABLE     = 0x002,
	MMU_PDE_USER         = 0x004,
        MMU_PDE_PWT          = 0x008,
	MMU_PDE_PCD          = 0x010,
	MMU_PDE_ACCESSED     = 0x020,
	MMU_PDE_DIRTY        = 0x040,
	MMU_PDE_4MB          = 0x080,
	MMU_PDE_CPU_GLOBAL   = 0x100,
	MMU_PDE_LV4_GLOBAL   = 0x200,
};

enum MMU_PTE_FLAGS {
	MMU_PTE_EMPTY         = 0x000,
	MMU_PTE_PRESENT       = 0x001,
	MMU_PTE_WRITABLE      = 0x002,
	MMU_PTE_USER          = 0x004,
	MMU_PTE_WRITETHROUGH  = 0x008,
	MMU_PTE_NOT_CACHEABLE = 0x010,
	MMU_PTE_ACCESSSED     = 0x020,
	MMU_PTE_DIRTY         = 0x040,
	MMU_PTE_PAT           = 0x080,
	MMU_PTE_CPU_GLOBAL    = 0x100,
	MMU_PTE_LV4_GLOBAL    = 0x200,
};

typedef uint32_t mmu_pde_t;
typedef mmu_pde_t mmu_page_directory_t[1024] __attribute((aligned(PAGESIZE)));

typedef uint32_t mmu_pte_t;
typedef mmu_pte_t mmu_page_table_t[1024] __attribute((aligned(PAGESIZE)));


#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)

void clear_page_directory(mmu_page_directory_t* dir);
void clear_page_table(mmu_page_table_t* table);

void load_page_directory(mmu_page_directory_t* phys_addr);

void mmu_map_page(mmu_page_directory_t* dir, void* phys_addr, void* virt_addr, uint32_t flags);

void mmu_flush_tlb(void* virt_addr);

void* V2P(void* virtualaddr);
