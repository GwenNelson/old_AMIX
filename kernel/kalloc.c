#include <kernel/kalloc.h>
#include <kernel/kmain.h>
#include <kernel/arch/mmu.h>
#include <kernel/arch/memlayout.h>
#include <kernel/spinlock.h>

#include <stdint.h>
#include <stddef.h>

#define PGSIZE 4096
#define PGROUNDUP(sz)  (((sz)+((uintptr_t)PGSIZE-1)) & ~((uintptr_t)(PGSIZE-1)))

void freerange(void* vstart, void* vend);

struct run {
    struct run *next;
};

struct {
    struct run *freelist;
} kmem;

spinlock_t kalloc_lock;

void init_kalloc() {
     init_lock(&kalloc_lock);
}

void freerange(void* vstart, void* vend) {

   	char* p;
     p = (char*)PGROUNDUP((uintptr_t)vstart);
     for(; p + PGSIZE <= (char*)vend; p += PGSIZE) kfree(p);

}

void kfree(char* v) {
     acquire_lock(&kalloc_lock);
   	struct run *r;
     r = (struct run*)v;
     r->next = kmem.freelist;
     kmem.freelist = r;
     release_lock(&kalloc_lock);
}

char* kalloc() {
     acquire_lock(&kalloc_lock);
      struct run *r;
      r = kmem.freelist;
      if(r) kmem.freelist = r->next;

     if(r==NULL) {
        kprintf("\n\nOOM\n\n");
	for(;;) asm volatile("cli; hlt");
     }
     release_lock(&kalloc_lock);
     return (char*)r;
}
