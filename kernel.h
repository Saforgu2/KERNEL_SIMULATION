#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

/* Esleitutako azken ID-adierazten du */
extern uint32_t global_id;
extern uint32_t global_time;
extern uint32_t global_tick;
extern uint32_t ct_end;
extern uint32_t ct_done;

extern pthread_mutex_t ct_mutex;
extern pthread_mutex_t tg_mutex;
extern pthread_mutex_t tsd_mutex;
extern pthread_cond_t  ct_cond;
extern pthread_cond_t  ct_cond2;


/*  HW-aren configutazioa*/
typedef struct HWC
{
    uint8_t cpus, cores, hwthreads;
    uint32_t clock_freq;     /*Hz-etan*/
    uint32_t timer_freq;    /*  Tick arteko erloju-zikloak*/
    uint32_t duration;      /*  Simulazioaren iraupena*/
} HWC;

extern HWC cfg;

// PCB-aren egitura
typedef struct PCB
{
    uint32_t id;
    uint32_t life_time;
    struct PCB *next;
} PCB;

/*ProcessQueue-ren egitura*/
typedef struct ProcessQueue
{
    PCB *first, *last;
    uint32_t size;
    pthread_mutex_t mutex;
} ProcessQueue;

void PQ_init (ProcessQueue *q);
void HWC_init ();
void global_init ();
void ProcessGenerator ();
void global_destroy ();
void *clock_run (void *arg);
void *timer_run (void *arg);

