#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "mem.h"

// Error code variable
int m_error = 0;

// Memory block structure
struct mem_block {
    size_t size;
    struct mem_block *next;
};

// Head of the free list
static struct mem_block *free_list = NULL;

// Initialize memory allocator
int mem_init(int size_of_region) {
    if (free_list != NULL || size_of_region <= 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    size_of_region = (size_of_region + sizeof(struct mem_block) - 1) / sizeof(struct mem_block) * sizeof(struct mem_block);
    free_list = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (free_list == MAP_FAILED) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    free_list->size = size_of_region - sizeof(struct mem_block);
    free_list->next = NULL;

    return 0;
}

// Allocate memory
void *mem_alloc(int size, int style) {
    if (free_list == NULL) {
        m_error = E_BAD_ARGS;
        return NULL;
    }

    size = (size + 7) / 8 * 8;

    struct mem_block *curr = free_list;
    struct mem_block *best_fit = NULL;
    size_t min_size = SIZE_MAX;

    while (curr != NULL) {
        if (curr->size >= size) {
            if (style == M_FIRSTFIT || style == M_BESTFIT) {
                if (curr->size < min_size) {
                    min_size = curr->size;
                    best_fit = curr;
                }
            } else if (style == M_WORSTFIT) {
                if (curr->size > min_size) {
                    min_size = curr->size;
                    best_fit = curr;
                }
            }
        }
        curr = curr->next;
    }

    if (best_fit == NULL) {
        m_error = E_NO_SPACE;
        return NULL;
    }

    void *allocated_mem = best_fit + 1;
    best_fit->size = size;

    if (style == M_BESTFIT || style == M_WORSTFIT) {
        best_fit->next = best_fit + size + sizeof(struct mem_block);
        best_fit->next->size = min_size - size - sizeof(struct mem_block);
        best_fit->next->next = NULL;
    }

    return allocated_mem;
}

// Free memory
int mem_free(void *ptr) {
    if (ptr == NULL) {
        return -1;
    }

    struct mem_block *block = (struct mem_block *)ptr - 1;

    struct mem_block *curr = free_list;
    struct mem_block *prev = NULL;

    while (curr != NULL && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    if (prev == NULL) {
        free_list = block;
    } else {
        prev->next = block;
    }

    block->next = curr;

    return 0;
}

// Dump memory
void mem_dump() {
    struct mem_block *curr = free_list;

    printf("Free memory dump:\n");

    while (curr != NULL) {
        printf("[size: %zu bytes]\n", curr->size);
        curr = curr->next;
    }
}
