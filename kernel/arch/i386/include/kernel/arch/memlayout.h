#pragma once

#define KERN_BASE ((uintptr_t)0xC0000000)

#define EARLY_P2V(addr) (((uintptr_t)(addr)) + (KERN_BASE))
#define EARLY_V2P(addr) (((uintptr_t)(addr)) - (KERN_BASE))

