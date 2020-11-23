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

void bench(int b, int min_size, int max_size, int percent_freed)
{
    void *array_r[b];

    long t0 = (unsigned long)time(NULL);

    struct timeval start;
    struct timeval end;

    gettimeofday(&start, NULL);

    int i;
    int p = percent_freed != 0 ? (int)(1 / ((float)percent_freed / 100)) : -1;

    for (i = 0; i < b; i++)
    {
        int r = psuedo_r(min_size, max_size);
        array_r[i] = dalloc(r);

        if (p != -1 && i % p != 0)
        {
            dfree(array_r[i]);
            array_r[i] = NULL;
        }
    }

    for (i = 0; i < b; i++)
    {
        if (array_r[i] != NULL)
            dfree(array_r[i]);
    }

    gettimeofday(&end, NULL);

    long d = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

    printf("%i,%lu\n", b, d);
}

int main(int argc, char *argv[])
{
    init();
    bench(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
}