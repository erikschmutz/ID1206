
#include "green.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#define PERIOD 100

int id = 0;
int cond_id = 0;

static sigset_t block;

void timer_handler(int);

static ucontext_t main_cntx;
static green_t main_green = {
    &main_cntx, // ucontext_t *context
    NULL,       // void *(*fun)(void *)
    NULL,       // void *arg
    NULL,       // struct green_t *next
    NULL,       // struct green_t *join
    NULL,       // void* retval
    FALSE,      // int zombie
    -1          // id of the process
};

int HAS_INITIALIZED = FALSE;

static green_t *running = &main_green;
static void init() __attribute__((constructor));

void init()
{

    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;

    assert(sigaction(SIGVTALRM, &act, NULL) == 0);

    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);

    getcontext(&main_cntx);
    HAS_INITIALIZED = TRUE;
}

void view_run_list()
{
    struct green_t *current = &main_green;

    while (current != NULL)
    {
        printf("{ id:%i, zombie: %i}", current->id, current->zombie);
        current = current->next;
    }

    printf("\n");
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
    if (n->join != NULL && n->join->zombie == FALSE)
        return n->join;

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
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;
    enqueue(susp);

    green_t *next = next_thread();
    running = next;

    swapcontext(susp->context, next->context);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

void timer_handler(int sig)
{

    // print("Interupt!", sizeof("Interupt!"));
    green_t *susp = running;
    enqueue(susp);

    green_t *next = next_thread();
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
    makecontext(cntx, (void (*)())green_thread, 0);

    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->retval = NULL;
    new->zombie = FALSE;
    new->id = id++;

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

void print(char *str, int length)
{
    write(1, str, length);
}

//

void add_to_list(struct green_list_node **original, green_t *context)
{
    struct green_list_node *current = *original;

    if (current == NULL)
    {
        (*original) = (struct green_list_node *)malloc(sizeof(struct green_list_node));
        (*original)->item = context;
        (*original)->next = NULL;

        return;
    }

    while (current != NULL && current->next != NULL)
    {
        current = current->next;
    }

    current->next = malloc(sizeof(struct green_list_node));
    current->next->item = context;
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

void green_cond_wait(green_cond_t *cond, green_mutex_t *mutex)
{

    sigprocmask(SIG_BLOCK, &block, NULL);

    add_to_list(&cond->list, running);
    green_t *prev = running;
    green_t *next = next_thread();

    if (mutex != NULL)
    {

        struct green_t *head = green_cond_dequeue(&mutex->list);

        if (head != NULL)
        {
            enqueue(head);
        }
        else
        {
            mutex->taken = FALSE;
        }
    }

    running = next;

    swapcontext(prev->context, next->context);

    if (mutex != NULL)
    {

        if (mutex->taken)
        {
            add_to_list(&mutex->list, running);
            green_t *prev = running;
            green_t *next = next_thread();

            running = next;

            swapcontext(prev->context, next->context);
        }
        else
        {
            mutex->taken = TRUE;
        }
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

struct green_t *green_cond_dequeue(struct green_list_node **original)
{
    struct green_list_node *head = *(original);

    if (head != NULL)
    {
        (*original) = (*original)->next;
        return head->item;
    }

    return NULL;
}

void green_cond_signal(green_cond_t *cond)
{

    struct green_t *head = green_cond_dequeue(&cond->list);

    if (head != NULL)
    {
        enqueue(head);
    }
}

//

int green_mutex_init(green_mutex_t *mutex)
{
    mutex->taken = FALSE;
    mutex->list = NULL;

    return 0;
}

int green_mutex_lock(green_mutex_t *mutex)
{

    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;

    if (mutex->taken)
    {
        add_to_list(&mutex->list, running);
        green_t *prev = running;
        green_t *next = next_thread();

        running = next;

        swapcontext(prev->context, next->context);
    }
    else
    {
        mutex->taken = TRUE;
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);

    return 0;
}

int green_mutex_unlock(green_mutex_t *mutex)
{

    sigprocmask(SIG_BLOCK, &block, NULL);

    struct green_t *head = green_cond_dequeue(&mutex->list);

    if (head != NULL)
    {
        enqueue(head);
    }
    else
    {
        mutex->taken = FALSE;
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}
