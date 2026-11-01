#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>

uint32_t global_id = 0;

void PQ_init(ProcessQueue *q)
{
    q->first = NULL;
    q->last = NULL;
    q->size = 0;
    pthread_mutex_init(&q->mutex, NULL);
}