
#include "green.h"
#include <stdlib.h>
#include <stdio.h>

int id = 0;
int cond_id = 0;

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
    HAS_INITIALIZED = TRUE;
}

void enqueue(green_t *thread)
{
    green_t *current = &main_green;

    while (current->next != NULL)
    {
        current = current->next;
    }

    current->next = thread;
}

struct green_t *dequeue()
{

    // main_green => a_green => b_green => c_green
    // Will return next_green and point main_green to
    // third green
    if (main_green.next == NULL)
        return &main_green;

    struct green_t *next = main_green.next;

    main_green.next = main_green.next->next;
    next->next = NULL;

    return next;
}

struct green_t *next_thread()
{

    green_t *n = dequeue();

    if (n == NULL)
        return NULL;
    if (n->zombie)
        return next_thread();

    return n;
}

int green_thread()
{

    //
    green_t *this = running;

    // Startat
    void *result = (this->fun)(this->arg);

    (*this).retval = result;

    // Sets the zombie value to true
    // since the function is complete
    this->zombie = TRUE;

    green_t *next = next_thread();
    running = next;

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

    green_t *next = next_thread();
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
    new->id = ++id;

    enqueue(new);

    return 0;
}

struct green_t *run_next()
{
    green_t *current = running;
    green_t *next = next_thread();
    running = next;
    swapcontext(current->context, next->context);
    return next;
}

int green_join(green_t *thread, void **res)
{

    struct green_t *next;

    if (!thread->zombie)
    {
        running->join = thread;
        next = run_next();
    }

    if (res != NULL)
    {
        *res = &thread->retval;
    }

    free(thread->context);

    return 0;
}

//
//
//

void add_observer(green_cond_t *cond, green_t *context)
{
    struct green_list_node *current = cond->list;

    if (current == NULL)
    {
        cond->list = (struct green_list_node *)malloc(sizeof(struct green_list_node));
        cond->list->item = context;
        cond->list->next = NULL;

        return;
    }

    while (current != NULL && current->next != NULL)
    {
        current = current->next;
    }

    current->next = malloc(sizeof(struct green_list_node));
    current->next->item = context;
    cond->list->next = NULL;
}

int len(struct green_list_node *list)
{

    int i = 0;
    struct green_list_node *current = list;

    while (current != NULL)
    {
        current = current->next;
        i++;
    }

    return i;
}

void green_cond_init(green_cond_t *cond)
{
    cond->list = NULL;
    cond->id = cond_id++;
}

void green_cond_wait(green_cond_t *cond)
{
    add_observer(cond, running);
}

void green_cond_signal(green_cond_t *cond)
{
    struct green_list_node *head = cond->list;

    if (head != NULL)
    {
        printf("I AM HERE");
        swapcontext(running->context, head->item->context);
    }
    else
    {
        green_yield();
    }
}