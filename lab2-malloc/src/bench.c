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

    for (i = 0; i < b / 4; i++)
    {
        int r = psuedo_r(0, b);
        dfree(array_r[r]);
    }

    gettimeofday(&end, NULL);
    long d = end.tv_usec - start.tv_usec;

    printf("took %lu us\n", d);
    printf("Length of my free list is : %i\n", free_list_length);
}

int main(int argc, char *argv[])
{
    init();
    bench(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
}