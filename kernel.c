#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "kernel.h"

//uint8_t  first_process;

uint32_t global_id;
uint32_t global_time;
uint32_t global_tick;
uint32_t ct_end;
uint32_t ct_done;
uint32_t tg_done;
uint32_t tsd_done;

pthread_mutex_t ct_mutex;
pthread_mutex_t tg_mutex;
pthread_mutex_t tsd_mutex;

pthread_cond_t  ct_cond;
pthread_cond_t  ct_cond2;
pthread_cond_t  tg_cond;
pthread_cond_t  tg_cond2;
pthread_cond_t  tsd_cond;
pthread_cond_t  tsd_cond2;

void PQ_init ()
{
    pq.first = NULL;
    pq.last = NULL;
    pq.size = 0;

    pqs.first = NULL;
    pqs.last = NULL;
    pqs.size = 0;

    pthread_mutex_init(&pq.mutex, NULL);
    pthread_mutex_init(&pqs.mutex, NULL);
}

void global_init ()
{
    //first_process = 1;

    global_id = 0;
    global_time = 0;
    global_tick = 0;
    
    ct_end = 0;
    ct_done = 0;
    tg_done = 0;
    tsd_done =0;

    pthread_mutex_init (&ct_mutex, NULL);
    pthread_mutex_init (&tg_mutex, NULL);
    pthread_mutex_init (&tsd_mutex, NULL);
    
    pthread_cond_init  (&ct_cond, NULL);
    pthread_cond_init  (&ct_cond2, NULL);
    pthread_cond_init  (&tg_cond, NULL);
    pthread_cond_init  (&tg_cond2, NULL);
    pthread_cond_init  (&tsd_cond, NULL);
    pthread_cond_init  (&tsd_cond2, NULL);

    srand(time(NULL));
}

void HWC_init ()
{
    uint32_t n;

    printf("Sartu cpus balioa: ");
    scanf("%u", &n);
    cfg.n_cpus= n;
    
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

    cpus_alloc();

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
    pthread_cond_destroy(&tg_cond2);
    pthread_cond_destroy(&tsd_cond);
    pthread_cond_destroy(&tsd_cond2);

    print_cpus();

    free_cpus();
}

void *clock_run (void *arg)
{
    pthread_mutex_lock(&ct_mutex);
    while (!ct_end)
    {
        while(ct_done < cfg.clock_freq) pthread_cond_wait(&ct_cond, &ct_mutex); // 1 = Hz
        //printf("Tick clock\n");
        ct_done = 0;
        global_time++;
        if (global_time >= cfg.duration) {
            ct_end = 1;
            //printf("clock\n");

            pthread_cond_signal(&tg_cond);
            pthread_cond_signal(&tsd_cond);   
        }
        pthread_cond_broadcast(&ct_cond2);
    }
    pthread_mutex_unlock(&ct_mutex);

    return NULL;
}

void *timer_run (void *arg) //numero de ciclos/ticks= timerfreq 5--> 1ciclo cada 4ticks
{
    uint32_t ct_cycles = 0;

    pthread_mutex_lock(&ct_mutex);
    while (!ct_end)
    {
        ct_done++;
        ct_cycles++;

        if (ct_cycles == cfg.timer_freq) {
            ct_cycles = 0;

            if (global_tick % cfg.process_freq == 0) {
                pthread_mutex_lock(&tg_mutex);
                tg_done = 1;
                pthread_cond_signal(&tg_cond);
                while (tg_done)
                    pthread_cond_wait(&tg_cond2, &tg_mutex);
                pthread_mutex_unlock(&tg_mutex);
            }

            pthread_mutex_lock(&tsd_mutex);
            tsd_done = 1;
            pthread_cond_signal(&tsd_cond);
            while (tsd_done)
                pthread_cond_wait(&tsd_cond2, &tsd_mutex);
            pthread_mutex_unlock(&tsd_mutex);

            global_tick++;
            //printf("Cycle\n");
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
        while (!tg_done && !ct_end)
            pthread_cond_wait(&tg_cond, &tg_mutex);

        if (!ct_end) {
            PCB *pcb = create_pcb();
            /*
            printf("------------------------------------\n");
            printf("PCB id: %u\n",       pcb->id);
            printf("PCB life_time: %u\n", pcb->life_time);
            printf("------------------------------------\n");
            */
            add_process_to_queue(pcb);

            tg_done = 0;
            pthread_cond_signal(&tg_cond2);
        }
    }
    pthread_mutex_unlock(&tg_mutex);
    print_pq();
    free_pq();
    return NULL;
}

void *scheduler_run(void *arg)
{
    pthread_mutex_lock(&tsd_mutex);
    while (!ct_end)
    {
        while (!tsd_done && !ct_end)
            pthread_cond_wait(&tsd_cond, &tsd_mutex);
        
        if(!ct_end){
            //printf("scheduler:  %u\n", global_tick);

            tsd_done = 0;
            pthread_cond_signal(&tsd_cond2);
        }
    }
    pthread_mutex_unlock(&tsd_mutex);
    return NULL;
}

PCB *create_pcb ()
{
    PCB *pcb = malloc(sizeof(PCB));
    uint8_t nice = rand() % sizeof(uint8_t);
    uint32_t ran_dur = rand() % cfg.duration;

    pcb->nice      = nice;
    pcb->id        = global_id;
    pcb->life_time = ran_dur;
    pcb->next      = NULL;
    pcb->behind    = NULL;

    global_id++;

    return pcb;
}

void add_process_to_queue (PCB *pcb)
{
    if (pq.first == NULL) {
        pq.first = pcb;
        pq.last  = pcb;
        
        //first_process = 0;
    }
    else {
        pcb->next         = pq.last;
        pcb->next->behind = pcb;
        pq.last           = pcb;
    }

    pcb->state = READY;
    pq.size++;
}

void cpus_alloc ()
{
    cfg.cpus = malloc(cfg.n_cpus * sizeof(CPU));
    uint8_t i, j;
    for (i = 0; i < cfg.n_cpus; i++)
    {
        cfg.cpus[i].threads = malloc(cfg.cores * cfg.hwthreads * sizeof(TCB));
        cfg.cpus[i].unused_threads = (cfg.cores * cfg.hwthreads);
        cfg.cpus[i].id = i;
        for (j = 0; j < (cfg.cores * cfg.hwthreads); j++)
        {
            cfg.cpus[i].threads[j].id  = j;
            cfg.cpus[i].threads[j].pcb = NULL;
        }
    }
}

void print_pq () 
{
    if (pq.size >= 1)
    {
        PCB *current = pq.last;
        while (current)
        {
            /*
            printf("PCB id: %u\n",       current->id);
            printf("PCB life_time: %u\n", current->life_time);
            printf("------------------------------------\n");
            */
            current = current->next;
        }
    }
}

void free_pq ()
{
    PCB *current = pq.last, *next;
    while (current)
    {
        next = current->next;
        free(current);

        current = next;
    }
    pq.last = pq.first = NULL;
    pq.size = 0;
}

void print_cpus ()
{
    uint8_t i, j;
    for (i = 0; i < cfg.n_cpus; i++)
    {
        printf("Cpua: %u\n", i);
        for (j = 0; j < (cfg.cores * cfg.hwthreads); j++)
        {
            printf("Hariare id: %u\n", cfg.cpus[i].threads[j].id);
        }
    }
}

void free_cpus ()
{   
    uint8_t i;
    for(i=0; i< cfg.n_cpus; i++)
    {
        free(cfg.cpus[i].threads);
    }
    free(cfg.cpus);
}