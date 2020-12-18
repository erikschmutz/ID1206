
#define _XOPEN_SOURCE 600
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "ucontext.h"

static int is_done = 0;
static ucontext_t cntx_main;

void run_one()
{
    int i = 0;
    printf("Starting run two");

    while (i < 5)
    {
        sleep(1);
        i++;
    }

    printf("done_one\n");
    is_done++;
}

void run_two()
{
    int i = 0;
    printf("Starting run two");

    while (i < 3)
    {
        sleep(1);
        i++;
    }

    printf("done two\n");
}

void test(void *args)
{
    printf("test!!1");

    return;
}

typedef struct action
{
    void (*func)(void *);
} action;

void _wait()
{
    while (is_done < 1)
    {
    }
}

ucontext_t *run(struct action *a)
{

    void *stack = malloc(sizeof(char) * 1024 + 1);

    ucontext_t *ctx = malloc(sizeof(ctx));
    getcontext(ctx);

    ctx->uc_link = &cntx_main;
    ctx->uc_stack.ss_sp = stack;
    ctx->uc_stack.ss_size = sizeof(char) * 1024;

    makecontext(ctx, a->func, 0);

    return ctx;
}

int main()
{

    struct action *a1 = malloc(sizeof(struct action));

    struct action *a2 = malloc(sizeof(struct action));

    a1->func = &run_one;
    a2->func = &run_two;

    // getcontext(&cntx_main);
    printf("TEST");
    ucontext_t *run_context = run(a1);
    // _wait();

    if (!run_context)
    {
        printf("ptr does not exists!");
    }
    else
    {
        printf("%p", run_context->uc_stack.ss_sp);
    }
    // {
    // }
    // printf("%i", run_context->uc_onstack);

    swapcontext(&cntx_main, run_context);
    // // run_one();
    // // run_two();
}