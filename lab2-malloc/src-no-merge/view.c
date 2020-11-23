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
    // print_state();
    print_state();

    void *test = dalloc(16);
    void *test2 = dalloc(128);

    dfree(test);
    dfree(test2);
    print_state();

    // print_state();
}