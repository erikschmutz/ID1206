
#include "green.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void test_runnable(void *arg)
{
    int i = *(int *)arg;
    int loop = 4;
    printf("tst...");
    while (loop > 0)
    {
        printf("thread %d : %d\n", i, loop);
        loop--;
        green_yield();
    }
}

void should_initlize()
{
    assert(HAS_INITIALIZED == TRUE);
}

void should_execute()
{

    // green_t g0, g1;
    // int a0 = 0, a1 = 1;

    // green_create(&g0, (void *(*)(void *))test_runnable, &a0);
    // green_create(&g1, (void *(*)(void *))test_runnable, &a1);
    // green_join(&g0, NULL);
    // green_join(&g1, NULL);
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

    green_t g0, g1;
    int a0 = 0, a1 = 1;

    green_create(&g0, (void *(*)(void *))test_runnable, &a0);
    green_create(&g1, (void *(*)(void *))test_runnable, &a1);

    // green_join(&g0, NULL);
    green_join(&g1, NULL);
}