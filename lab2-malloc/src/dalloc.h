#include <stdlib.h>

void dfree(void *memory);
void *dalloc(size_t request);
void print_state();
void init();
int free_list_length;