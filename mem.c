#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

/* Memory error codes */
#define E_NO_SPACE            1
#define E_CORRUPT_FREESPACE   2
#define E_PADDING_OVERWRITTEN 3
#define E_BAD_ARGS            4
#define E_BAD_POINTER         5

/* Memory allocation styles */
#define M_BESTFIT   0
#define M_WORSTFIT  1
#define M_FIRSTFIT  2

/* Memory error variable */
int m_error;

/* Memory block structure */
struct mem_block {
    size_t size;
    struct mem_block *next;
};

/* Global variables */
static struct mem_block *free_list = NULL;

/* Memory initialization function */
int mem_init(int size_of_region) {
    if (size_of_region <= 0) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    /* Round up size_of_region to page size */
    int page_size = getpagesize();
    int num_pages = (size_of_region + page_size - 1) / page_size;
    size_of_region = num_pages * page_size;

    free_list = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (free_list == MAP_FAILED) {
        m_error = E_BAD_ARGS;
        return -1;
    }

    free_list->size = size_of_region - sizeof(struct mem_block);
    free_list->next = NULL;

    return 0;
}

/* Memory allocation function */
void *mem_alloc(int size, int style) {
    if (size <= 0) {
        m_error = E_BAD_ARGS;
        return NULL;
    }

    struct mem_block *prev = NULL;
    struct mem_block *curr = free_list;
    struct mem_block *best_block = NULL;

    while (curr != NULL) {
        if (curr->size >= size && (best_block == NULL || (style == M_BESTFIT && curr->size < best_block->size) || (style == M_WORSTFIT && curr->size > best_block->size))) {
            best_block = curr;
            if (style == M_FIRSTFIT) {
                break;
            }
        }
        prev = curr;
        curr = curr->next;
    }

    if (best_block == NULL) {
        m_error = E_NO_SPACE;
        return NULL;
    }

    if (best_block->size - size >= sizeof(struct mem_block)) {
        struct mem_block *new_block = (struct mem_block *)((uintptr_t)best_block + sizeof(struct mem_block) + size);
        new_block->size = best_block->size - size - sizeof(struct mem_block);
        new_block->next = best_block->next;
        best_block->next = new_block;
    }

    if (prev != NULL) {
        prev->next = (void *)((uintptr_t)best_block + size + sizeof(struct mem_block));
    } else {
        free_list = (void *)((uintptr_t)best_block + size + sizeof(struct mem_block));
    }

    return (void *)((uintptr_t)best_block + sizeof(struct mem_block));
}

/* Memory free function */
int mem_free(void *ptr) {
    if (ptr == NULL) {
        return 0;
    }

    struct mem_block *block = (struct mem_block *)((uintptr_t)ptr - sizeof(struct mem_block));
    struct mem_block *prev = NULL;
    struct mem_block *curr = free_list;

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

/* Memory dump function */
void mem_dump() {
    printf("Memory dump after allocation:\n");
    struct mem_block *curr = free_list;
    while (curr != NULL) {
        printf("[size: %zu bytes]\n", curr->size);
        curr = curr->next;
    }
}

int main() {
    /* Initialize memory allocator */
    int init_result = mem_init(4096); /* Initialize with 4KB of memory */
    if (init_result == -1) {
        printf("Memory initialization failed\n");
        return 1;
    }

    /* Allocate memory */
    void *ptr1 = mem_alloc(16, M_FIRSTFIT);
    if (ptr1 == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    /* Free memory */
    mem_free(ptr1);

    /* Dump memory */
    mem_dump();

    return 0;
}
