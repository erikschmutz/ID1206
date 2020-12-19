
#include "green.h"
#include <stdlib.h>
#include <stdio.h>
#define FALSE 0
#define TRUE 1
#define STACK_SIZE 4096
static ucontext_t main_cntx = {0};

static green_t main_green = {
    &main_cntx, // ucontext_t *context
    NULL,       // void *(*fun)(void *)
    NULL,       // void *arg
    NULL,       // struct green_t *next
    NULL,       // struct green_t *join
    NULL,       // void* retval
    FALSE       //  int zombie
};

static green_t *running = &main_green;
static void init() __attribute__((constructor));

void init()
{
    getcontext(&main_cntx);
}

int green_thread()
{
    green_t *this = running;

    void *result = (*this->fun)(this->arg);

    return 0;
}

int green_yield()
{
    return 0;
}

void enqueue(green_t *thread)
{

    green_t *t = &main_green;

    while (t->next != NULL)
    {
        t = t->next;
    }
    t->next = thread;
}

struct green_t *dequeue()
{
    if (main_green.next == NULL)
    {
        printf("This is the final thread");
        return &main_green;
    }

    struct green_t *ready = main_green.next;
    main_green.next = ready->next;
    ready->next = NULL;
    return ready;
}

void green_thread()
{

    green_t *this = running;
    void *result = (this->fun)(this->arg);
    this->retval = result;
    this->zombie = TRUE;
    green_t *next = dequeue();
    running = next;
    setcontext(next->context);
}

void green_yield()
{
    green_t *susp = running;
    enqueue(susp);
    green_t *next = dequeue();
    running = next;

    swapcontext(susp->context, next->context);
}

int green_create(green_t *new, void *(*fun)(void *), void *arg)
{
    ucontext_t *cntx = (ucontext_t *)malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);
    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, green_thread, 0);

    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->retval = NULL;
    new->zombie = FALSE;

    running->next = NULL;
    running = new;

    return 0;
}

int green_join(green_t *thread, void **res)
{
    if (!thread->zombie)
    {
        green_t *susp = running;
        susp->join = thread;
        enqueue(susp);
        green_t *next = susp->next;
        running = next;
        swapcontext(susp->context, next->context);
    }

    res = running->retval;

    free(thread->context);

    return 0;
}

int main()
{
    calc_heavy();
    calc_heavy();
    calc_heavy();
    return 0;
}