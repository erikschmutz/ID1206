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

    int i;
    int p = percent_freed != 0 ? (int)(1 / ((float)percent_freed / 100)) : -1;

    for (i = 0; i < b; i++)
    {
        int r = psuedo_r(min_size, max_size);
        array_r[i] = dalloc(r);
    }

    for (i = 0; i < b * 3 / 4; i++)
    {
        int d = psuedo_r(0, b);

        if (array_r[d] != NULL)
        {
            dfree(array_r[d]);
        }
    }

    printf("%i,%i\n", i, free_list_length);
}

int main(int argc, char *argv[])
{
    init();
    bench(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
}