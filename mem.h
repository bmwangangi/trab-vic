// mem.h
#ifndef __MEM_H_
#define __MEM_H_

#define E_NO_SPACE            1
#define E_CORRUPT_FREESPACE   2
#define E_PADDING_OVERWRITTEN 3
#define E_BAD_ARGS            4
#define E_BAD_POINTER         5

#define M_BESTFIT   0
#define M_WORSTFIT  1
#define M_FIRSTFIT  2

extern int m_error;

int mem_init(int size_of_region);
void * mem_alloc(int size, int style);
int mem_free(void * ptr);
void mem_dump();

#endif

// mem.c
#include "mem.h"
#include <stdio.h>
#include <sys/mman.h>

struct mem_block {
    size_t size;
    struct mem_block *next;
};

static void *free_list = NULL;

int m_error = 0;

int mem_init(int size_of_region) {
    if (free_list != NULL || size_of_region <= 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    free_list = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (free_list == MAP_FAILED) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    struct mem_block *block = (struct mem_block *)free_list;
    block->size = size_of_region - sizeof(struct mem_block);
    block->next = NULL;

    return 0;
}

void * mem_alloc(int size, int style) {
    // Implementation of mem_alloc
    return NULL;
}

int mem_free(void *ptr) {
    // Implementation of mem_free
    return 0;
}

void mem_dump() {
    // Implementation of mem_dump
}

// main.c
#include <stdio.h>
#include "mem.h"

int main() {
    // Initialize memory allocator with a size
    if (mem_init(4096) == -1) {
        printf("Memory initialization failed!\n");
        return 1;
    }

    // Allocate memory
    void *ptr1 = mem_alloc(16, M_FIRSTFIT);
    void *ptr2 = mem_alloc(32, M_FIRSTFIT);

    // Free memory
    mem_free(ptr1);

    // Dump memory
    mem_dump();

    // Free memory
    mem_free(ptr2);

    // Dump memory
    mem_dump();

    return 0;
}
