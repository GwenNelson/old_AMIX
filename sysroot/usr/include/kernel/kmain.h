#pragma once

#include <stdint.h>
#include <stddef.h>

#include <kernel/timer.h>

void kmain(void* alloc_pool, size_t alloc_pool_size, timer_t* timer);
