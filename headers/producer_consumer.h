/***************
 * 
 * file : producer_consumer.h
 * 
 * This file contains all the structures used in the producer_consumer 
 * problem implementation.
 * 
 ***************/

#ifndef CONSUMER_PRODUCER
#define CONSUMER_PRODUCER

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

/**
 * @brief This structure is going to represent a circular buffer, used in the producer consumer 
 *          imoplementation.
 * 
 * @param empty (my_sem_t * / sem_t *) : Semaphore consumers wait on if the buffer is empty.
 * @param full  (my_sem_t * / sem_t *) : Semaphore producers wait on if the buffer is full.
 * @param mutex (my_mut_t * / pthread_mutex_t * ) : Mutex to lock the critical section.
 */
typedef struct 
{
    signed long * array;
#ifdef MY_SEMAPHORE // If we imported our implementation of a semaphore
    my_sem_t * empty;
    my_sem_t * full;
    my_mut_t * mutex;
#else 
    sem_t * empty;
    sem_t * full;
    pthread_mutex_t * mutex;
#endif
    uint8_t takePointer;
    uint8_t putPointer;
}circular_buf;

typedef struct 
{
    circular_buf * buffer;
}prod_args;

typedef struct 
{
    circular_buf * buffer;
}cons_args;

#endif