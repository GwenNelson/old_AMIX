#pragma once

#include <stdint.h>
#include <stddef.h>

#include <kernel/timer.h>

#include <kernel/arch/mmu.h>
mmu_page_directory_t kernel_page_dir;

void kmain(void* alloc_pool, size_t alloc_pool_size, timer_t* timer);
