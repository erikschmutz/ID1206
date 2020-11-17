#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0
#define DALLOC_SIZE_ERROR 1
#define HEAD (sizeof(struct head))
#define MIN(size) (((size) > (8)) ? (size) : (8))
#define LIMIT(size) (MIN(0) + HEAD + size)
#define MAGIC(memory) ((struct head *)memory - 1)
#define HIDE(block) (void *)((struct head *)block + 1)
#define ALIGN 8
#define ARENA (64 * 1024)

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

    block->next = flist;
    block->prev = NULL;

    if (flist != NULL)
    {
        flist->prev = block;
    }

    flist = block;
}

void printState()
{

    struct head *current = flist;

    while (current != NULL)
    {
        printf("> %i, size %i\n", current->id, current->size);
        current = current->next;
    };
}

struct head *after(struct head *block)
{
    return (struct head *)((long)block + (block->size + HEAD));
}

struct head *before(struct head *block)
{
    return (struct head *)(block - (HEAD + block->size));
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
    new->bfree = TRUE;
    new->bsize = 0;
    new->free = TRUE;
    new->size = size;

    struct head *sentinel = after(new);

    sentinel->bfree = TRUE;
    sentinel->bsize = size;
    sentinel->free = 0;
    sentinel->size = 0;

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

    printf("Requesting dalloc for size %i\n", size);

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
            taken->size = taken->size - r + HEAD;

            // Here we split the block
            struct head *r_block = after(taken);
            r_block->size = r;
            r_block->free = TRUE;

            insert(r_block);
        }

        detach(taken);

        return taken;
    }
    //
}

void test_adjust()
{
    printf("3 %i\n", adjust(3));
    printf("5 %i\n", adjust(5));
    printf("10 %i\n", adjust(10));
    printf("13 %i\n", adjust(13));
    printf("15 %i\n", adjust(15));
    printf("31 %i\n", adjust(31));
    printf("99 %i\n", adjust(99));
}

void test_dalloc()
{

    new ();
    insert(arena);

    struct head *my_head = dalloc(sizeof(struct head));
    printState();
    printf("%p\n", my_head);
}

int main()
{
    struct head *first;

    test_dalloc();
    // first = new ();
    /**
     * 
     *  u_int16_t bfree;
    u_int16_t bsize;
    u_int16_t id;
    u_int16_t free;
    u_int16_t size;

    struct head *next;
    struct head *prev;
     * */

    struct head *head1 = malloc(sizeof(struct head));
    head1->id = 0;

    struct head *head2 = malloc(sizeof(struct head));
    head2->id = 1;

    struct head *head3 = malloc(sizeof(struct head));
    head3->id = 7;
}
