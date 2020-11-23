#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include "dalloc.h"

#define TRUE 1
#define FALSE 0
#define HEAD (sizeof(struct head))
#define MIN(size) (((size) > (8)) ? (size) : (8))
#define LIMIT(size) (MIN(0) + HEAD + size)
#define MAGIC(memory) ((struct head *)memory - 1)
#define HIDE(block) (void *)((struct head *)block + 1)
#define ALIGN 8
#define ARENA (64 * 1024)

int free_list_length = 0;
int block_id = 0;
struct head
{
    u_int16_t bfree;
    u_int16_t bsize;
    u_int16_t id;
    u_int16_t free;
    u_int16_t size;

    struct head *next;
    struct head *prev;
};

struct head *arena = NULL;
struct head *flist;

void detach(struct head *block)

{
    free_list_length--;
    if (block->next != NULL)
    {
        block->next->prev = block->prev;
    }

    // Really ugly case statement...
    if (block->prev != NULL)
    {
        block->prev->next = block->next;
    }
    else
    {
        flist = block->next;
    }
}

void insert(struct head *block)
{
    free_list_length++;
    block->next = flist;
    block->prev = NULL;

    if (flist != NULL)
    {
        flist->prev = block;
    }

    flist = block;
}

struct head *after(struct head *block)
{
    return (struct head *)((long)block + (block->size + HEAD));
}

void view_head(struct head *current)
{
    printf("%i, size %i, free: %i, bfree: %i, bsize: %i, pos: %p\n", current->id, current->size, current->free, current->bfree, current->bsize, current);
}

void print_state()
{

    printf("------\n");
    struct head *current = flist;

    while (current != NULL)
    {
        printf("List ");
        view_head(current);
        current = current->next;
    };

    current = arena;
    int x = 0;
    printf("\n");

    while (((long)current) < ((long)arena + ARENA))
    {

        printf("Block ");
        view_head(current);
        current = after(current);
    }

    printf("\n");
}

struct head *before(struct head *block)
{
    return (struct head *)((long)block - (HEAD + block->bsize));
}

struct head *split(struct head *block, int size)
{
    int rsize = block->size - HEAD - size;
    block->size = rsize;

    struct head *splt = after(block);
    splt->bsize = block->size;
    splt->bfree = block->free;
    splt->size = size;
    splt->free = size;

    struct head *aft = after(splt);
    aft->bsize = splt->size;

    return splt;
}

struct head *new ()
{

    if (arena != NULL)
    {
        printf("one arena already allocated\n");
        return NULL;
    }

    // ARG1: NULL->Kernel chooses virtual address
    // ARG2: Second arguments, size of the new request area of the heap in bytes
    // ARG3: Sets read/write access
    // ARG4: Config, allows to set certains flags on the area
    // ARG5: Sets if the memory is going to be shared
    // ARG6: Allows you to offset the pointer
    struct head *new = mmap(NULL, ARENA, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new == MAP_FAILED)
    {
        printf("mmap failed : error %d\n", errno);
        return NULL;
    }

    uint32_t size = ARENA - 2 * HEAD;
    new->bfree = FALSE;
    new->bsize = 0;
    new->free = TRUE;
    new->size = size;
    new->id = 20;

    struct head *sentinel = after(new);

    sentinel->bfree = TRUE;
    sentinel->bsize = size;
    sentinel->free = 0;
    sentinel->size = 0;
    sentinel->id = 100;

    arena = (struct head *)new;

    return new;
}

int adjust(size_t requestedSize)
{

    int k = requestedSize / ALIGN;
    int r = requestedSize % ALIGN;

    return k * ALIGN + (r == 0 ? 0 : ALIGN);
}

struct head *find(int size)
{
    struct head *current = flist;

    while (current != NULL)
    {
        if (current->free == TRUE && current->size >= size)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}

void *dalloc(size_t request)
{

    if (request <= 0)
        return NULL;

    int size = adjust(request);
    struct head *taken = find(size);

    if (taken == NULL)
        return NULL;
    else
    {
        int cs = taken->size;
        int r = cs - size - HEAD;

        if (r > ALIGN)
        {

            // we resize the block
            taken->size = size;

            // Here we split the block
            struct head *r_block = after(taken);

            r_block->size = r;
            r_block->free = TRUE;

            r_block->bfree = FALSE;
            r_block->bsize = size;

            insert(r_block);
        }

        taken->bsize = before(taken)->size;
        taken->bfree = FALSE;

        taken->free = FALSE;
        taken->id = ++block_id;

        detach(taken);

        return taken + HEAD;
    }
    //
}

struct head *merge(struct head *block)
{
    struct head *aft = after(block);

    if (block->bfree)
    {

        struct head *b = before(block);
        u_int16_t n_size = b->size + block->size + HEAD;

        b->size = n_size;
        aft->bsize = b->size;

        detach(b);
        block = b;
    }

    if (aft->free)
    {
        u_int16_t n_size = aft->size + block->size + HEAD;

        block->size = n_size;
        block->prev = aft->prev;
        detach(aft);
    }

    return block;
}

void dfree(void *memory)
{
    if (memory != NULL)
    {
        struct head *block = ((struct head *)memory) - HEAD;
        struct head *aft = after(block);

        //
        block = merge(block);

        block->free = TRUE;
        aft->bfree = TRUE;

        insert(block);
    }

    // Merge mememory

    return;
}

void init()
{
    new ();
    insert(arena);
}

// int main()
// {
//     printf("Please do not call the dalloc file directly\n");
//     return 1;
// }