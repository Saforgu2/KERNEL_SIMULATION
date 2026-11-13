#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>

#define READY    0
#define RUNNING  1
#define FINISHED 2

#define QUANTUM  10

/* Esleitutako azken ID-adierazten du */
extern uint32_t global_id; 
extern uint32_t global_time;
extern uint32_t global_tick;
extern uint32_t ct_end;
extern uint32_t ct_done;
extern uint32_t tg_done;
extern uint32_t tsd_done;


extern pthread_mutex_t ct_mutex;
extern pthread_mutex_t tg_mutex;
extern pthread_mutex_t tsd_mutex;
extern pthread_cond_t  ct_cond;
extern pthread_cond_t  ct_cond2;
extern pthread_cond_t  tg_cond;
extern pthread_cond_t  tg_cond2;

// PCB-aren egitura
typedef struct PCB
{
    uint8_t state;
    uint8_t nice;
    uint32_t id;
    uint32_t life_time;
    struct PCB *next;
    struct PCB *behind;
} PCB;

typedef struct TCB
{
    uint32_t id;
    PCB *pcb;
} TCB;

typedef struct CPU
{
    uint32_t id;
    TCB *threads;
} CPU;

/*  HW-aren configutazioa*/
typedef struct HWC
{
    CPU *cpus;
    uint8_t  n_cpus;
    uint8_t  cores, hwthreads;
    uint32_t clock_freq;     /*Hz-etan*/
    uint32_t timer_freq;    /*  Tick arteko erloju-zikloak*/
    uint32_t duration;      /*  Simulazioaren iraupena*/
    uint32_t process_freq;
    uint8_t threads_per_cpu;
} HWC;

extern HWC cfg;

/*ProcessQueue-ren egitura*/
typedef struct ProcessQueue
{
    PCB *first, *last;
    uint32_t size;
    pthread_mutex_t mutex;
} ProcessQueue;

extern ProcessQueue pq;

typedef struct ProcessQueueSpecial
{
    PCB *first, *last;
    uint32_t size;
    pthread_mutex_t mutex;
} ProcessQueueSpecial;

extern ProcessQueueSpecial pqs;

typedef struct FinishedQueue
{
    PCB *first, *last;
    uint32_t size;
    pthread_mutex_t mutex;
} FinishedQueue;

extern FinishedQueue fq;

void PQ_init ();
void HWC_init ();
void global_init ();
void ProcessGenerator ();
void global_destroy ();
void *clock_run (void *arg);
void *timer_run (void *arg);
void *generate_process (void *arg);
void *scheduler_run (void *arg);
void add_process_to_queue (PCB *pcb);
void print_pq ();
void free_pq ();
void free_cpus ();
void cpus_alloc ();
void print_cpus ();

PCB *create_pcb ();
