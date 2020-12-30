
#include "green.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int test_runnable(void *arg)
{
    int i = *(int *)arg;
    int loop = 4;
    while (loop > 0)
    {
        loop--;
        green_yield();
    }

    return 100 + i;
}

void should_initlize()
{
    assert(HAS_INITIALIZED == TRUE);
}

void should_execute()
{

    green_t g0, g1, g2;
    int a0 = 0, a1 = 1, a2 = 2;

    green_create(&g0, (void *(*)(void *))test_runnable, &a0);
    green_create(&g1, (void *(*)(void *))test_runnable, &a1);
    green_create(&g2, (void *(*)(void *))test_runnable, &a2);

    int *r0 = malloc(sizeof(int)), *r1 = malloc(sizeof(int)), *r2 = malloc(sizeof(int));

    *r0 = -1, *r1 = -2, *r2 = -3;

    assert(*r0 == -1);
    assert(*r1 == -2);
    assert(*r2 == -3);

    free(r0);
    free(r1);

    green_join(&g0, &r0);
    green_join(&g1, &r1);

    assert(*r0 == 100);
    assert(*r1 == 101);
    assert(*r2 == -3);

    free(r2);
    green_join(&g2, &r2);
    assert(*r2 == 102);
}

void should_enque_and_dequeue()
{

    struct green_t *a = (struct green_t *)malloc(sizeof(struct green_t));
    struct green_t *b = (struct green_t *)malloc(sizeof(struct green_t));
    struct green_t *c = (struct green_t *)malloc(sizeof(struct green_t));

    a->id = 1;
    b->id = 2;
    c->id = 3;

    green_create(a, (void *(*)(void *))test_runnable, NULL);
    green_create(b, (void *(*)(void *))test_runnable, NULL);
    green_create(c, (void *(*)(void *))test_runnable, NULL);

    assert(dequeue()->id == 1);
    assert(dequeue()->id == 2);
    assert(dequeue()->id == 3);

    // Should not remove sentinel
    assert(dequeue()->id == -1);
    assert(dequeue()->id == -1);
}

int main()
{

    should_initlize();
    should_enque_and_dequeue();
    should_execute();
}