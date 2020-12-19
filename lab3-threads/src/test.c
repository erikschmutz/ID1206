
#include "green.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void test_runnable()
{
}

void should_initlize()
{

    // assert(main_cntx.uc_mcontext != 0);
    assert(HAS_INITIALIZED == TRUE);
}

void should_execute()
{
    struct green_t *a = (struct green_t *)malloc(sizeof(struct green_t));
    green_create(a, (void *(*)(void *))test_runnable, NULL);

    a->id = 123;
    green_thread();
}

void should_enque_and_dequeue()
{

    struct green_t *a = (struct green_t *)malloc(sizeof(struct green_t));
    struct green_t *b = (struct green_t *)malloc(sizeof(struct green_t));
    struct green_t *c = (struct green_t *)malloc(sizeof(struct green_t));

    green_create(a, (void *(*)(void *))test_runnable, NULL);
    green_create(b, (void *(*)(void *))test_runnable, NULL);
    green_create(c, (void *(*)(void *))test_runnable, NULL);

    a->id = 1;
    b->id = 2;
    c->id = 3;

    enqueue(a);
    enqueue(b);
    enqueue(c);

    assert(dequeue()->id == 1);
    assert(dequeue()->id == 2);
    assert(dequeue()->id == 3);
}

int main()
{
    should_initlize();
    should_enque_and_dequeue();
    should_execute();
}