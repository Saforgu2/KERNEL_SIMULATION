#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include "kernel.h"

HWC cfg;
ProcessQueue pq;
ProcessQueueSpecial pqs;
FinishedQueue fq;


int main(int argc, char *argv[])
{
    pthread_t clock, timer, process_generator, sched_disp;

    HWC_init();
    PQ_init();
    global_init();

    pthread_create(&clock, NULL, clock_run, NULL);
    pthread_create(&timer, NULL, timer_run, NULL);
    pthread_create(&process_generator, NULL, generate_process, NULL);
    pthread_create(&sched_disp, NULL, scheduler_run, NULL);

    pthread_join(clock, NULL);
    pthread_join(timer, NULL);
    pthread_join(process_generator, NULL);
    pthread_join(sched_disp, NULL);

    global_destroy();
    return 0;
}
