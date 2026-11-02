#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"

uint32_t global_id;
uint32_t global_tick;
uint32_t global_time;

pthread_mutex_t ct_mutex;
pthread_mutex_t tg_mutex;
pthread_mutex_t tsd_mutex;

void PQ_init (ProcessQueue *q)
{
    q->first = NULL;
    q->last = NULL;
    q->size = 0;

    pthread_mutex_init(&q->mutex, NULL);
}


void global_init ()
{
    global_id = 0;
    global_tick = 0;
    global_time = 0;

    pthread_mutex_init (&ct_mutex, NULL);
    pthread_mutex_init (&tg_mutex, NULL);
    pthread_mutex_init (&tsd_mutex, NULL);

}

void HWC_init (HWC *cfg)
{
    printf("Sartu cpus balioa: ");
    scanf("%d", &n);
    cfg.cpus= n;
    
    printf("Sartu cores balioa: ");
    scanf("%u", &n);
    cfg.cores = n;

    printf("Sartu hwthreads balioa: ");
    scanf("%u", &n);
    cfg.hwthreads = n;

    printf("Sartu clock-freq balioa: ");
    scanf("%u", &n);
    cfg.clock_freq = n;

    printf("Sartu timer-freq balioa: ");
    scanf("%u", &n);
    cfg.timer_freq = n;

    printf("Sartu duration balioa: ");
    scanf("%u", &n);
    cfg.duration = n;

    HWC_ini(&cfg);
    printf("egitura hasieratuta");
}