#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "kernel.h"

HWC cfg;
ProcessQueue pq;

int main(int argc, char *argv[])
{
    pthread_t clock, timer, process_generator, sched_disp;

    HWC_init(&cfg);
    PQ_init(&pq);
    global_init();
}
