#pragma once

// this file specifies the standard timer interface

typedef struct timer_t timer_t;
typedef struct timer_t {
	void (*callback)();
} timer_t;
