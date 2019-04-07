#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/debug_output.h>
#include <kernel/multiboot.h>
#include <kernel/arch/memlayout.h>
#include <kernel/kmain.h>
#include <kernel/printf.h>
#include <kernel/arch/gdt.h>
#include <kernel/arch/idt.h>
#include <kernel/arch/pic.h>
#include <kernel/arch/cpuid.h>

#define CHECK_FLAG(flags,mask)   ((flags) & mask)

/*
 * We do platform-specific stuff here and get ready for the kernel to take over
 * 
 */

extern char* _kernel_start;
extern char* _kernel_end;

static char cmdline[1024];

void* alloc_pool = NULL;
size_t alloc_pool_size = 0;

#define DEFAULT_CMDLINE "verbose"

void handle_cmdline(struct multiboot_info* mboot_ptr) {
   
     if(CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_CMDLINE)) {
        snprintf(cmdline,1024,"%s",(char*)EARLY_P2V(mboot_ptr->cmdline));
     } else { // provide a default command line
        snprintf(cmdline,1024,"%s",DEFAULT_CMDLINE);
     }

     // TODO: add support for actually parsing the command line here
     kprintf("Command line: %s\n",cmdline);

}

void handle_mmap(struct multiboot_info* mboot_ptr) {
     size_t free_space = 0;
     
     size_t kernel_size = ((uintptr_t)&_kernel_end) - ((uintptr_t)&_kernel_start);
   	if(CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_MEM_MAP)) {

  	     struct multiboot_mmap_entry* mb_mmap;

           mboot_ptr->mmap_addr = EARLY_P2V(mboot_ptr->mmap_addr);
	   mb_mmap = (struct multiboot_mmap_entry*)mboot_ptr->mmap_addr;
	   kprintf("Memory map at 0x%p\n", mb_mmap);

	   while(mb_mmap < (mboot_ptr->mmap_addr + mboot_ptr->mmap_length)) {
                uint32_t start_addr = mb_mmap->base_addr_low;
                uint32_t end_addr   = start_addr + mb_mmap->length_low;
                if(end_addr < start_addr) end_addr = 0xFFFFFFFF;
		if(end_addr < 0x100000) mb_mmap->type++;
		if(start_addr == 0x0)   mb_mmap->type++;
		if((start_addr >= EARLY_V2P((uintptr_t)&_kernel_start))) { 
		   start_addr += kernel_size + 4096;
		   mb_mmap->length_low -= kernel_size;
		}
		kprintf("MULTIBOOT MMAP 0x%08x - 0x%08x 0x%08x %d\n", start_addr, end_addr, mb_mmap->length_low, mb_mmap->type);
		if(mb_mmap->type == 1) { 
			alloc_pool  = (void*)start_addr;
			free_space += mb_mmap->length_low;
		}
		mb_mmap = (struct multiboot_mmap_entry*) ( (unsigned int)mb_mmap + mb_mmap->size + sizeof(mb_mmap->size) );
           }
     }
     alloc_pool_size = free_space-kernel_size;

     kprintf("Kernel image is at 0x%p to 0x%p, %ikb\n", &_kernel_start, &_kernel_end, kernel_size / 1024);
     kprintf("Free space: %ikb\n", (free_space-kernel_size)/1024);
}

void handle_ram(struct multiboot_info* mboot_ptr) {
     if(CHECK_FLAG(mboot_ptr->flags, MULTIBOOT_INFO_MEMORY)) {
        uint32_t mem_lower = mboot_ptr->mem_lower;
	uint32_t mem_upper = mboot_ptr->mem_upper;
        kprintf("Bootloader tells us we have %ikb lower RAM and %ikb higher RAM\n", mem_lower, mem_upper);
     }
}

void x86_enter(struct multiboot_info *mboot_ptr) {
     kprintf("AMIX booting on i386\n");
     handle_cmdline(mboot_ptr);
     handle_ram(mboot_ptr);
     handle_mmap(mboot_ptr);

     init_gdt();
     init_idt();
     init_pic();
     init_pit();
     asm volatile("sti");

     // TODO - pass a timing interface to kmain, kmain can then setup tasking
     kmain(alloc_pool+4096,alloc_pool_size);
}
