/**************
 * 
 * file : tas_producer_consumer.c
 * 
 * This code was inspired by the consumer-producer pseudo-code seen during 
 * the lectures of LINFO1252 - Operating systems. Except that instead of using
 * POSIX synchronisation primitive types we are going to use our implementation of active 
 * wait synchronisation primitives.
 * 
 ***************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "test_and_set.h"
#include "my_semaphore.h"
#include "producer_consumer.h"

int MAX = 1024;
volatile int count_produced = 0;
volatile int count_consumed = 0;

my_mut_t mutex;

/**
 * @brief Prints error message to the stderr and exit's the program.
 * 
 * @param err (int)    : The integer returned by the failed function. 
 * @param msg (char *) : The name of the failed function.
 */
void error(int err, char *msg)
{
    fprintf(stderr,"%s returned an error : %d, the message is : %s \n", msg, err, strerror(errno));
    exit(EXIT_FAILURE);
}

void process(){ while(rand() > RAND_MAX/10000); }

/**
 * @brief This function inserts an item in the circular buffer.
 * 
 * @param buff (circular_buf *) : The circular buffer.
 * @param item (signed long)    : The item to insert in the buffer.
 */
void insert_item(circular_buf * buff, signed long item)
{
    my_sem_wait(buff->empty);
    my_mut_lock(buff->mutex);
    buff->array[buff->putPointer] = item;
    buff->putPointer = (buff->putPointer+1) % 8;
    my_mut_unlock(buff->mutex);
    my_sem_post(buff->full);
}

/**
 * @brief Implements the producer's behaviour
 * 
 * @param arg (prod_args * arg)  : The producer's argument structure.
 * @return NULL
 */
void * producer (void * arg)
{
    prod_args * args = (prod_args *) arg;
    my_mut_lock(&mutex);
    int signal = count_produced < MAX;
    my_mut_unlock(&mutex);

    while (signal)     
    {
        my_mut_lock(args->buffer->mutex);
        count_produced++;
        my_mut_unlock(args->buffer->mutex);

        insert_item(args->buffer, rand());
        process();
        
        my_mut_lock(&mutex);
        signal = count_produced < MAX;
        my_mut_unlock(&mutex);
    }
    return(NULL);
}

/**
 * @brief Get the item out of the circular buffer.
 * 
 * @param buff (circular_buf *) : The circular buffer.
 * @param item (signed long * ) : The pointer to hold the result.
 */
void get_item(circular_buf * buff, signed long * item)
{
    my_sem_wait(buff->full);
    my_mut_lock(buff->mutex);
    *item = buff->array[buff->takePointer];
    buff->takePointer = (buff->takePointer+1) % 8;
    my_mut_unlock(buff->mutex);
    my_sem_post(buff->empty);
}

/**
 * @brief Implemements the consumer's behaviour.
 * 
 * @param arg (cons_args *) : The producer's argument structure.
 * @return NULL
 */
void * consumer(void * arg)
{
    cons_args * args = (cons_args *) arg;
    signed long holder;
    my_mut_lock(&mutex);
    int signal = count_consumed < MAX;
    my_mut_unlock(&mutex);

    while ( signal )   // Signal that there's still items to consume
    {
        my_mut_lock(args->buffer->mutex);
        count_consumed++;
        my_mut_unlock(args->buffer->mutex);

        get_item(args->buffer, &holder);
        process();

        my_mut_lock(&mutex);
        signal = count_consumed < MAX;
        my_mut_unlock(&mutex);
    }
    return (NULL);
}

int main(int argc, char const *argv[])
{
    uint32_t NBTHREADS = fmax(atoi(argv[1]), 2);;
    uint32_t PRODUCERS = NBTHREADS/2;
    uint32_t CONSUMERS = (NBTHREADS % 2 == 0) ? NBTHREADS/2 : (NBTHREADS/2) + 1;
    signed long * array[8];

    my_mut_t buffer_mutex;
    my_sem_t empty;
    my_sem_t full;

    pthread_t prods[PRODUCERS];
    pthread_t conss[CONSUMERS];

    circular_buf buffer;
    prod_args arg_for_prod;
    cons_args arg_for_cons;

    int err;

    // Initiate the random generator with the process id as the seed
    srand(getpid()); 

    my_mut_init(&buffer_mutex);
    my_mut_init(&mutex);
    my_sem_init(&empty, 8);
    my_sem_init(&full, 0);

    buffer = (circular_buf) { (signed long *) &array, &empty, &full, &buffer_mutex, 0, 0};

    arg_for_prod = (prod_args) {&buffer};
    arg_for_cons = (cons_args) {&buffer};

    for (size_t i = 0; i < PRODUCERS; i++)
    {
        err = pthread_create(&prods[i], NULL, producer, (void *) &(arg_for_prod));
        if (err != 0)
            error(err, "pthread_create");
    }

    for (size_t i = 0; i < CONSUMERS; i++)
    {
        err = pthread_create(&conss[i], NULL, consumer, (void *) &(arg_for_cons));
        if (err != 0)
            error(err, "pthread_create");
    }
    
    for (size_t i = 0; i < PRODUCERS; i++)
    {
        err = pthread_join(prods[i], NULL);
        if (err != 0)
            error(err, "pthread_join");
    }

    for (size_t i = 0; i < CONSUMERS; i++)
    {
        err = pthread_join(conss[i], NULL);
        if (err != 0)
            error(err, "pthread_join");
    }
    
    my_mut_destroy(&buffer_mutex);
    my_sem_destroy(&empty);
    my_sem_destroy(&full);

    return (EXIT_SUCCESS);
}
