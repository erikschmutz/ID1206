
#include "green.h"
#include <stdlib.h>
#include <stdio.h>

static ucontext_t main_cntx;

static green_t main_green = {
    &main_cntx, // ucontext_t *context
    NULL,       // void *(*fun)(void *)
    NULL,       // void *arg
    NULL,       // struct green_t *next
    NULL,       // struct green_t *join
    NULL,       // void* retval
    FALSE,      //  int zombie
    -1          // id of the process
};

int HAS_INITIALIZED = FALSE;

static green_t *running = &main_green;
static void init() __attribute__((constructor));

void init()
{

    getcontext(&main_cntx);
    printf(">>%i\n", main_cntx.uc_mcontext);
    HAS_INITIALIZED = TRUE;
}

void enqueue(green_t *thread)
{
    green_t *current = &main_green;

    while (current->next != NULL)
        current = current->next;

    current->next = thread;
}

struct green_t *dequeue()
{

    // main_green => next_green => third_green
    // Will return next_green and point main_green to
    // third green
    if (main_green.next == NULL)
        return &main_green;

    struct green_t *ready = main_green.next;
    main_green.next = ready->next;
    ready->next = NULL;

    return ready;
}

int green_thread()
{

    //
    green_t *this = running;

    // Startat
    void *result = (this->fun)(this->arg);
    this->retval = result;

    // Sets the zombie value to true
    // since the function is complete
    this->zombie = TRUE;

    green_t *next = dequeue();
    running = next;

    printf("3>>%i\n", next->context->uc_mcontext);

    if (next == NULL)
    {
        printf("But wait there no next...");
    }

    setcontext(next->context);

    return 0;
}

int green_yield()
{
    green_t *susp = running;
    enqueue(susp);
    green_t *next = dequeue();
    running = next;

    swapcontext(susp->context, next->context);

    return 0;
}

int green_create(green_t *new, void *(*fun)(void *), void *arg)
{
    ucontext_t *cntx = (ucontext_t *)malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);
    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, (void (*)())green_thread, 0);

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