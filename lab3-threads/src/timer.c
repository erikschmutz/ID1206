#include "green.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#define PERIOD 100

void timer_handler(int);

static sigset_t block;

void init()
{

    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;

    assert(sigaction(SIGVTALRM, &act, NULL) == 0);

    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);
}

void timer_handler(int sig)
{
    write(1, "sasdasdasdasasdsadsadd", sizeof("sasdasdasdasasdsadsadd"));
}

int main()
{
    write(1, "starte!...", sizeof("started..."));
    init();
    while (TRUE)
    {
    }
}