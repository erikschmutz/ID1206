
#define _XOPEN_SOURCE 600
#include "stdlib.h"
#include "ucontext.h"
#include "green.h"
#include "stdlib.h"
#include "stdio.h"

typedef struct green_t
{
    ucontext_t *context;
    void *(*fun)(void *);
    void *arg;
    struct green_t *next;
    struct green_t *join;
    int zombie;
} green_t;

int green_create(green_t *thread, void *(*fun)(void *), void *arg);
int green_yield();
int green_join();