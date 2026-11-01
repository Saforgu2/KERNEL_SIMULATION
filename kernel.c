#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

// Esleitutako azken ID-adierazten du
uint32_t global_id;

// HW-aren configutazioa
typedef HWC
{
    uint8_t cpus, cores, hwthreads;
    uint8_t clock_freq;     // Hz-etan
    uint32_t timer_freq;    // Tick arteko erloju-zikloak
    uint32_t duration;      // Simulazioaren iraupena
}

// PCB-aren egitura
typedef struct PCB
{
    uint32_t id;
    uint32_t life_time;
    struct PCB *next;
} PCB;

// ProcessQueue-ren egitura
typedef struct ProcessQueue
{
    PCB *first, *last;
    uint32_t size;
    pthread_mutex_t mutex;
} ProcessQueue;

void PQ_init (ProcessQueue *q);

int main ()
{
    ProcessQueue pq;
    PQ_init(&pq);
}

void PQ_init (ProcessQueue *q)
{
    q->first = NULL;
    q->last  = NULL;
    pthread_mutex_init(&p->mutex);
}