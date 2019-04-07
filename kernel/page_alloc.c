#include <kernel/page_alloc.h>
#include <kernel/arch/memlayout.h>
#include <kernel/debug_output.h>

#include <assert.h>

void page_alloc_init(page_alloc_allocator_t* allocator, bool use_lock, void (*init_lock_func)(void**), void (*lock_func)(void*), void (*unlock_func)(void*)) {
	allocator->freelist = NULL;
	allocator->use_lock = use_lock;
	allocator->lock     = lock_func;
	allocator->unlock   = unlock_func;
	if(use_lock) {
		init_lock_func(&(allocator->lock_data));
	}
}

// this sets up a region ready for use for allocating pages
void page_alloc_init_region(page_alloc_allocator_t* allocator, void* region_start, size_t len) {
      region_start += KERN_BASE;
  	page_alloc_region_t *r, *rend;
      page_alloc_region_t **rp;
      page_alloc_region_t *p, *pend;
      if(allocator->use_lock) allocator->lock(allocator->lock_data);

      p    = (page_alloc_region_t*)region_start;
      pend = (page_alloc_region_t*)(region_start + len);

      for(rp=&allocator->freelist; (r=*rp) != 0 && r <= pend; rp=&r->next) {
	  rend = (page_alloc_region_t*)((void*)r + r->len);
	  assert(!(r <= p & p < rend));

	  if(rend == p) { // r before p: expand r to include p
		r->len += len;
		if(r->next && r->next==pend) { // r next to r->next?
			r->len += r->next->len;
			r->next = r->next->next;
		}
		goto out;
	  }
	  if(pend == r) { // p before r: expand p to include+replace r
		p->len  = len + r->len;
		p->next = r->next;
		*rp = p;
		goto out;
	  }

      }
      // insert p before r in freelist
      p->len  = len;
      p->next = r;
      *rp = p;
out:
      if(allocator->use_lock) allocator->unlock(allocator->lock_data);
}

void page_alloc_free(page_alloc_allocator_t* allocator, void* region_start, size_t len) {
     page_alloc_init_region(allocator,region_start, len);
}

size_t page_alloc_count_free_pages(page_alloc_allocator_t* allocator) {
      assert(allocator->freelist != NULL);
      size_t retval=0;
      page_alloc_region_t* i;
      if(allocator->use_lock) allocator->lock(allocator->lock_data);
      for(i=allocator->freelist; i != NULL; i=i->next) {
	  retval += (i->len)/PAGESIZE;
      }
      if(allocator->use_lock) allocator->unlock(allocator->lock_data);
      return retval;
}

void* page_alloc(page_alloc_allocator_t* allocator, size_t len) {
      kprintf("freelist=%08p\n", allocator->freelist);
      void* p;
     page_alloc_region_t *r, **rp;
     assert((len % PAGESIZE) >= 0);

     if(allocator->use_lock) allocator->lock(allocator->lock_data);

     if((len % PAGESIZE) != 0) { // round it up to the next page boundary
       len = (len & ~(PAGESIZE-1)) + PAGESIZE;
     }

     kprintf("len=%i\n", len);
     for(rp=&(allocator->freelist); (r=*rp) != 0; rp=&r->next) {
         if(r->len >= len) {
            r->len -= len;
            p = (char*)r + r->len;
            if(r->len == 0) *rp = r->next;
            if(allocator->use_lock) allocator->unlock(allocator->lock_data);
            kprintf("p=0x%08p, rp=0x%08p, *rp=0x%08p\n", p, rp, *rp);
	    return p;
	 } else {
            kprintf("OH NO\n");
	 }
     }
     if(allocator->use_lock) allocator->unlock(allocator->lock_data);
     return NULL;
}
