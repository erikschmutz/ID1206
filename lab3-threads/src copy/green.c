
#define _XOPEN_SOURCE 600
#include "ucontext.h"
#include "green.h"
#include "stdlib.h"
#include "stdio.h"
#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

void *calc_heavy()
{

    int i = 0;

    while (i < 1000000)
    {
        i++;
        /* code */
    }
}

int green_create(green_t *thread, void *(*fun)(void *), void *arg)
{
    return 0;
}

int green_yield()
{
    return 0;
}

int green_join()
{
    return 0;
}

int main()
{
    calc_heavy();
    calc_heavy();
    calc_heavy();
    return 0;
}