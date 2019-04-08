#include <stdint.h>
#include <stddef.h>
#include <kernel/arch/idt.h>
#include <kernel/arch/tasking.h>
#include <kernel/arch/mmu.h>
#include <kernel/arch/memlayout.h>
#include <kernel/kalloc.h>
#include <kernel/syscalls.h>

int sys_debug_out(char c) {
    kprintf("%c",c);
    return 0;
}

int sys_debug_out_num(uintptr_t n) {
    kprintf("0x%08x",n);
}

uint32_t sys_get_tid() {
    uint32_t tid = running_task->tid;
    return tid;
}


extern char* usercode;
uint32_t sys_fork() {
	// TODO - iterate through stuff, copy relevant bits
	asm volatile("cli");
	task_control_block_t* new_task = (task_control_block_t*)kalloc();
	mmu_page_directory_t* new_pd   = (mmu_page_directory_t*)kalloc();

	mmu_map_page(&(running_task->regs.cr3),EARLY_V2P(new_task),new_task,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);
	mmu_map_page(&(running_task->regs.cr3),EARLY_V2P(new_pd),new_pd,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

	clear_page_directory(new_pd);

	int i=0;
	for(i=0; i<1024; i++) {
		mmu_map_page(new_pd,(void*)(i * 0x1000), (void*)(i * 0x1000) + KERN_BASE,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_CPU_GLOBAL|MMU_PTE_USER);
	}
	mmu_map_page(new_pd,0xb8000,0xC03FF000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

	char* p=(char*)kalloc();
	__builtin_memcpy(p,&usercode,4096);

	mmu_map_page(new_pd,V2P(p),0x00200000,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);

//	mmu_map_page(new_pd,new_task->regs.esp-4096,running_task->regs.esp-4096,MMU_PTE_WRITABLE|MMU_PTE_PRESENT|MMU_PTE_USER);


	create_task(new_task, running_task->regs.eip ,DEFAULT_TASK_FLAGS,V2P(new_pd));

	add_task(new_task);

	return new_task->tid;
}

uintptr_t (*syscalls_table[SYSCALL_COUNT+1])(uintptr_t,uintptr_t,uintptr_t,uintptr_t) = {
#define X(num,name) [num] &sys_##name,
	#include <kernel/syscalls.def>
#undef X
};

ISR(syscall_i80_handler) {
	uint32_t* userstack  = frame->useresp;
	uint32_t syscall_no  = userstack[0];

	if(syscall_no >0 && syscall_no < (sizeof(syscalls_table)/sizeof(void*)) && syscalls_table[syscall_no]) {
		running_task->regs.eip = __builtin_extract_return_addr(__builtin_return_address(0));
		userstack[0] = syscalls_table[syscall_no](userstack[1],userstack[2],userstack[3],userstack[4]);
	} else {
		dump_frame(frame);
		kprintf("unknown syscall number 0x%x\n",syscall_no);
	}

}
