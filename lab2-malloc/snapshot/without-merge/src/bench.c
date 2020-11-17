#include "dalloc.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

int psuedo_r(int min, int max)
{
    if (min == max)
        return min;

    return min + rand() % (max - min);
}

void bench(int b, int min_size, int max_size)
{
    void *array_r[b];
    printf("Benching for %i blocks between %i and %i ", b, min_size, max_size);

    long t0 = (unsigned long)time(NULL);

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    int i;
    for (i = 0; i < b; i++)
    {
        int r = psuedo_r(min_size, max_size);
        array_r[i] = dalloc(r);
    }

    for (i = 0; i < b; i++)
    {
        dfree(array_r[i]);
    }

    gettimeofday(&end, NULL);
    long d = end.tv_usec - start.tv_usec;

    printf("took %lu us\n", d);
}

int main(int argc, char *argv[])
{
    init();
    bench(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
}