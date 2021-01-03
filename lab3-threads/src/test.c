
#include "green.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#define LOUD FALSE
int flag = 0;
struct green_cond_t cond;
struct green_cond_t pub_cond;
struct green_cond_t ack_cond;

struct green_mutex_t mutex;

int message = 1;
int x = 0;

int sleep_runnable(void *arg)
{

    int id = *(int *)arg;

    long y = 0;
    while (TRUE)
        y = y % 2;

    return 100 + id;
}

int test_runnable(void *arg)
{

    int i = *(int *)arg;
    int loop = 4;

    while (loop > 0)
    {
        loop--;
        green_yield();
    }

    if (LOUD)
    {
        print("done", sizeof("done"));
    }

    return 100 + i;
}

int test_mutex_runnable(void *arg)
{

    int id = *(int *)arg;

    int i = 0;

    for (i = 0; i < 100000000; i++)
    {
        green_mutex_lock(&mutex);
        x++;
        green_mutex_unlock(&mutex);
    }
}

void test_condition(void *arg)
{

    int id = *((int *)arg);
    int loop = 4;

    while (loop > 0)
    {
        if (flag == id)
        {

            loop--;
            flag = (id + 1) % 2;

            // printf("thread %i : %i\n", id, loop);
            green_cond_signal(&cond);
        }
        else
        {
            green_cond_wait(&cond);
        }
    }

    return;
}

int get_fibb(int prev)
{

    int x = 0, y = 1;

    while (TRUE)
    {
        if (x > prev)
        {
            return x;
        }

        int t = x;
        x = x + y;
        y = t;
    }
}

void test_condition_producer()
{

    while (TRUE)
    {
        sleep(1);
        message = get_fibb(message);

        printf("P: Sent msg (%i)\n", message);
        green_cond_signal(&pub_cond);
        printf("P: Waiting for ack...\n");
        green_cond_wait(&ack_cond);
        printf("P: Recieved ack\n\n");
    }
}

void test_condition_consumer()
{
    int prev_message;

    while (TRUE)
    {
        green_cond_wait(&pub_cond);
        printf("S: Recieved msg(%i)\n", message);
        green_cond_signal(&ack_cond);
        printf("S: Sent ack...\n");
    }
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

    assert(dequeue()->id == 0);
    assert(dequeue()->id == 1);
    assert(dequeue()->id == 2);

    // Should not remove sentinel
    assert(dequeue()->id == -1);
    assert(dequeue()->id == -1);
}

void should_be_able_to_create_condition()
{

    struct green_cond_t *condition = (struct green_cond_t *)malloc(sizeof(struct green_cond_t));

    green_cond_init(condition);
    assert(len(condition->list) == 0);

    green_cond_wait(condition);
    assert(1 == len(condition->list));

    green_cond_dequeue(&condition->list);
    assert(0 == len(condition->list));
}

void should_be_able_to_execute_with_condition()
{
    int a0 = 0, a1 = 1;
    green_t g0, g1;

    green_create(&g0, (void *(*)(void *))test_condition, &a0);
    green_create(&g1, (void *(*)(void *))test_condition, &a1);

    green_cond_init(&cond);

    green_join(&g1, NULL);
    green_join(&g0, NULL);
}

void should_be_able_to_execute_a_pub_sub()
{
    int a0 = 0, a1 = 1;
    green_t g0, g1;

    green_create(&g0, (void *(*)(void *))test_condition_consumer, &a0);
    green_create(&g1, (void *(*)(void *))test_condition_producer, &a1);

    green_cond_init(&pub_cond);
    green_cond_init(&ack_cond);

    green_join(&g0, NULL);
    green_join(&g1, NULL);
}

void should_not_be_able_to_freeze()
{

    green_t g0, g1, g2;
    int a0 = 0, a1 = 1, a2 = 2;

    green_create(&g0, (void *(*)(void *))sleep_runnable, &a0);
    green_create(&g1, (void *(*)(void *))test_runnable, &a1);

    green_join(&g0, NULL);
    green_join(&g1, NULL);
}

void should_be_able_to_use_mutex()
{

    int a0 = 0, a1 = 1, a2 = 2, a3 = 3;
    green_t g0, g1, g2, g3;

    green_mutex_init(&mutex);

    green_create(&g0, (void *(*)(void *))test_mutex_runnable, &a0);
    green_create(&g1, (void *(*)(void *))test_mutex_runnable, &a1);
    green_create(&g2, (void *(*)(void *))test_mutex_runnable, &a2);
    green_create(&g3, (void *(*)(void *))test_mutex_runnable, &a3);

    green_join(&g0, NULL);
    green_join(&g1, NULL);
    green_join(&g2, NULL);
    green_join(&g3, NULL);

    printf("%i", (x));
}

int main()
{

    should_initlize();
    should_enque_and_dequeue();
    should_execute();
    should_be_able_to_create_condition();
    should_be_able_to_execute_with_condition();
    // should_be_able_to_execute_a_pub_sub();
    // should_not_be_able_to_freeze();

    should_be_able_to_use_mutex();
}