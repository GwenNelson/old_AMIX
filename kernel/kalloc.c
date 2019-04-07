#include <kernel/kalloc.h>

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

void freerange(void* vstart, void* vend) {
     char* p;
     p = (char*)PGROUNDUP((uintptr_t)vstart);
     for(; p + PGSIZE <= (char*)vend; p += PGSIZE) kfree(p);

}

void kfree(char* v) {
     struct run *r;
     r = (struct run*)v;
     r->next = kmem.freelist;
     kmem.freelist = r;
}

char* kalloc() {
      struct run *r;
      r = kmem.freelist;
      if(r) kmem.freelist = r->next;
      return (char*)r;
}
