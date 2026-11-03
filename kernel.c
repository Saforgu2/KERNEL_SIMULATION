#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"

uint32_t global_id;
uint32_t global_time;
uint32_t global_tick;
uint32_t ct_end;
uint32_t timer_n;

pthread_mutex_t ct_mutex;
pthread_mutex_t tg_mutex;
pthread_mutex_t tsd_mutex;

pthread_cond_t  ct_cond;
pthread_cond_t  ct_cond2;

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
    timer_n = 0;
    global_tick = 0;
    ct_end = 0;

    pthread_mutex_init (&ct_mutex, NULL);
    pthread_mutex_init (&tg_mutex, NULL);
    pthread_mutex_init (&tsd_mutex, NULL);
    
    pthread_cond_init  (&ct_cond, NULL);
    pthread_cond_init  (&ct_cond2, NULL);
}

void HWC_init (HWC *cfg)
{
    uint32_t n;

    printf("Sartu cpus balioa: ");
    scanf("%d", &n);
    cfg->cpus= n;
    
    printf("Sartu cores balioa: ");
    scanf("%u", &n);
    cfg->cores = n;

    printf("Sartu hwthreads balioa: ");
    scanf("%u", &n);
    cfg->hwthreads = n;

    printf("Sartu clock-freq balioa: ");
    scanf("%u", &n);
    cfg->clock_freq = n;

    printf("Sartu timer-freq balioa: ");
    scanf("%u", &n);
    cfg->timer_freq = n;

    printf("Sartu duration balioa: ");
    scanf("%u", &n);
    cfg->duration = n;

    printf("egitura hasieratuta");
}

void *clock_run (void *arg)
{
    while (1) //bukleko buelta bakoitza= ziklo bat
    {
        pthread_mutex_lock(&ct_mutex);
        while(done < timer_n) pthread_cond_wait(&ct_cond, &ct_mutex); 
        printf("Tick clock\n");
        done = 0;
        global_time++; //el numero de ciclo en el q estamos
        if (global_time >= cfg->duration) {
            ct_end = 1;
        }
        pthread_cond_broadcast(&ct_cond2);
        pthread_mutex_unlock(&ct_mutex);
        if(ct_end) break;
    }
}

void timer_run ()
{
    uint32_t cycles = 0;
    uint32_t seen = 0; //ultimo ciclo q se proceso. sirve para saber si seguimos en el mismo ciclo
    pthread_mutex_lock(&ct_mutex); 

    while (1)
    {
        done++;
        pthread_cond_signal(&ct_cond);
        while (global_time == seen && !ct_end)
            pthread_cond_wait(&ct_cond2, &ct_mutex); //si sigue en el mismo ciclo espera hasta q avance el clock
        if(ct_end) break;
        seen = global_time; //cilo nuevo
        cycles++; // cuenta 1ciclo
        if(cycles == cfg->timer_freq){
            cycles = 0;
            global_tick++; //cada timer_freq ciclos suela el tick
        }
    }
    pthread_mutex_unlock(&ct_mutex);
}


    