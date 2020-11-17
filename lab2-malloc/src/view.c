#include "dalloc.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    init();

    void *test = dalloc(10);
    void *test2 = dalloc(10);
    dfree(test);

    print_state();
}