
int main()
{

    int i;
    void *ptrs[ARENA / 16];

    for (i = 0; i < ARENA / 16; i++)
    {
        ptrs[i] = malloc(16);
    }

    for (i = 0; i < ARENA / 16; i++)
    {
        free(ptrs[i]);
    }

    return 0;
}