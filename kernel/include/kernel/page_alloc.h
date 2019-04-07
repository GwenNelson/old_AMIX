#pragma once
#ifndef __PAGE_ALLOC_H_
#define __PAGE_ALLOC_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGESIZE 4096

typedef struct page_alloc_region_t page_alloc_region_t;
typedef struct page_alloc_region_t {
	page_alloc_region_t* next;
	size_t len;
} page_alloc_region_t;

typedef struct page_alloc_allocator_t page_alloc_allocator_t;
typedef struct page_alloc_allocator_t {
	page_alloc_region_t* freelist;
	bool use_lock;
	void* lock_data;
	void (*lock)(void* lock_data);
	void (*unlock)(void* lock_data);
} page_alloc_allocator_t;

void page_alloc_init(page_alloc_allocator_t* allocator, bool use_lock, void (*init_lock_func)(void**),
								       void (*lock_func)(void*),
	                                                               void (*unlock_func)(void*));
void page_alloc_init_region(page_alloc_allocator_t* allocator, void* region_start, size_t len);
void* page_alloc(page_alloc_allocator_t* allocator, size_t len);
void page_alloc_free(page_alloc_allocator_t* allocator, void* region_start, size_t len);
size_t page_alloc_count_free_pages(page_alloc_allocator_t* allocator);



#endif
