#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"

uint32_t global_id;
uint32_t global_time;
uint32_t global_tick;
uint32_t ct_end;
uint32_t ct_done;

pthread_mutex_t ct_mutex;
pthread_mutex_t tg_mutex;
pthread_mutex_t tsd_mutex;

pthread_cond_t  ct_cond;
pthread_cond_t  ct_cond2;
pthread_cond_t  tg_cond;
pthread_cond_t  tg_cond2;

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
    global_time = 0;
    global_tick = 0;
    
    ct_end = 0;
    ct_done = 0;

    pthread_mutex_init (&ct_mutex, NULL);
    pthread_mutex_init (&tg_mutex, NULL);
    pthread_mutex_init (&tsd_mutex, NULL);
    
    pthread_cond_init  (&ct_cond, NULL);
    pthread_cond_init  (&ct_cond2, NULL);
}

void HWC_init ()
{
    uint32_t n;

    printf("Sartu cpus balioa: ");
    scanf("%u", &n);
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

    printf("Sartu prozesuak sortzeko frekuentzia balioa: ");
    scanf("%u", &n);
    cfg.process_freq = n;

    printf("egitura hasieratuta\n");
}

void global_destroy(void)
{
    pthread_mutex_destroy(&ct_mutex);
    pthread_mutex_destroy(&tg_mutex);
    pthread_mutex_destroy(&tsd_mutex);

    pthread_cond_destroy(&ct_cond);
    pthread_cond_destroy(&ct_cond2);
    pthread_cond_destroy(&tg_cond);
}

void *clock_run (void *arg)
{
    pthread_mutex_lock(&ct_mutex);
    while (!ct_end)
    {
        while(ct_done < cfg.clock_freq) pthread_cond_wait(&ct_cond, &ct_mutex); // 1 = Hz
        printf("Tick clock\n");
        ct_done = 0;
        global_time++;
        if (global_time >= cfg.duration) {
            ct_end = 1;
        }
        pthread_cond_broadcast(&ct_cond2);
    }
    pthread_mutex_unlock(&ct_mutex);

    return NULL;
}

void *timer_run (void *arg)
{
    uint32_t ct_cycles = 0;

    pthread_mutex_lock(&ct_mutex);
    while (!ct_end)
    {
        ct_done++;
        ct_cycles++;

        if (ct_cycles == cfg.timer_freq) {
            ct_cycles = 0;

            pthread_mutex_lock(&tg_mutex);
            pthread_cond_signal(&tg_cond);
            pthread_cond_wait(&tg_cond2, &tg_mutex);

            global_tick++;
            printf("Cycle\n");
        }

        if (ct_done >= cfg.clock_freq) {
            pthread_cond_signal(&ct_cond);
            while (ct_done >= cfg.clock_freq && !ct_end)
                pthread_cond_wait(&ct_cond2, &ct_mutex);
        }
    }
    pthread_mutex_unlock(&ct_mutex);

    return NULL;
}


void *generate_process (void *arg)
{
    pthread_mutex_lock(&tg_mutex);
    while (!ct_end)
    {
        pthread_cond_wait(&tg_cond, &tg_mutex);
        if (global_tick % cfg.process_freq == 0) {
            printf("Noggers\n");
            pthread_cond_signal(&tg_cond2);
        }
    }
    pthread_mutex_unlock(&tg_mutex);
    return NULL;
}
