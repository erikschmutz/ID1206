
#define _XOPEN_SOURCE 600
#include <ucontext.h>
#define FALSE 0
#define TRUE 1
#define STACK_SIZE 4096

typedef struct green_t
{
    ucontext_t *context;  // Pretty straight forward please in read in man to understand
    void *(*fun)(void *); // Callback function for the runnable
    void *arg;            // Arguments for the callback function
    struct green_t *next; // The runnable which is going to be executed after this runnable
    struct green_t *join; // The runnable which need to be completed before this one
    void *retval;         // Pointer return value
    int zombie;           // We do not understand this one yet.
    int id;               // For testing purposes
} green_t;

typedef struct green_list_node
{
    struct green_list_node *next;
    struct green_t *item;
} green_list_node;

typedef struct green_cond_t
{
    struct green_list_node *list;
    void *condition;
    int id;
} green_cond_t;

// {f(args..), condition = Date.now() > 1000000} => {f2(args2...), conditon = }

int green_yield();
int green_thread();
int green_create(green_t *thread, void *(*fun)(void *), void *arg);
int green_join();

void green_cond_init(green_cond_t *cond);
void green_cond_wait(green_cond_t *cond);
void green_cond_signal(green_cond_t *cond);

// For testing
struct green_t *dequeue();
void enqueue(green_t *thread);
int HAS_INITIALIZED;
static ucontext_t main_cntx;
int len(struct green_list_node *list);
struct green_t *green_cond_dequeue(green_cond_t *cond);