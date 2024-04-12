#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include "mem.h"

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

struct mem_block {
    size_t size;
    struct mem_block *next;
};

static struct mem_block *free_list = NULL;
static int m_error = 0;

int mem_init(int size_of_region) {
    if (free_list != NULL || size_of_region <= 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    // Round up the size_of_region to the nearest multiple of PAGE_SIZE
    size_of_region = ((size_of_region + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

    // Allocate memory region using mmap()
    free_list = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (free_list == MAP_FAILED) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    // Initialize free_list
    free_list->size = size_of_region - sizeof(struct mem_block);
    free_list->next = NULL;

    return 0;
}

void *mem_alloc(int size, int style) {
    if (free_list == NULL) {
        m_error = E_BAD_ARGS;
        return NULL;
    }

    // Align size to multiple of 8
    size = (size + 7) / 8 * 8;

    struct mem_block *prev = NULL;
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
        prev = curr;
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

int mem_free(void *ptr) {
    if (ptr == NULL) {
        return 0;
    }

    struct mem_block *block = (struct mem_block *)ptr - 1;

    struct mem_block *prev = NULL;
    struct mem_block *curr = free_list;

    while (curr != NULL && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    if (prev != NULL) {
        prev->next = block;
    } else {
        free_list = block;
    }

    block->next = curr;

    if (curr != NULL && (char *)block + block->size + sizeof(struct mem_block) == (char *)curr) {
        block->size += sizeof(struct mem_block) + curr->size;
        block->next = curr->next;
    }

    return 0;
}

void mem_dump() {
    struct mem_block *curr = free_list;
    printf("Free memory dump:\n");
    while (curr != NULL) {
        printf("[size: %zu bytes]\n", curr->size);
        curr = curr->next;
    }
}

int main() {
    // Example usage
    if (mem_init(4096) == -1) {
        printf("Memory initialization failed!\n");
        return 1;
    }

    void *ptr1 = mem_alloc(16, M_FIRSTFIT);
    if (ptr1 == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    void *ptr2 = mem_alloc(4048, M_FIRSTFIT);
    if (ptr2 == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    mem_dump();

    mem_free(ptr1);
    printf("Memory dump after freeing ptr1:\n");
    mem_dump();

    return 0;
}
