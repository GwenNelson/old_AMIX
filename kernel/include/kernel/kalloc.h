#pragma once

void init_kalloc();
void freerange(void* vstart, void* vend);
char*           kalloc(void);
void            kfree(char*);
