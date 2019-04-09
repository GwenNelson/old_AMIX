#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct spinlock_t {
	uint8_t locked;
	uint32_t f;
} spinlock_t;

void init_lock(spinlock_t* l);
bool try_acquire_lock(spinlock_t* l);
void acquire_lock(spinlock_t* l);
void release_lock(spinlock_t* l);
