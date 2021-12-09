/***********
 * 
 * This code implements the reader writer problem as seen in the practical classes
 * of LINFO1252 - Operating Systems. Except that instead of using
 * POSIX synchronisation primitive types we are going to use our implementation of active 
 * wait synchronisation primitives.
 * 
 ***********/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "test_and_test_and_set.h"
#include "my_semaphore.h"

static int readcount = 0;
static int writecount = 0;

static my_mut_t mutex_readcount;
static my_mut_t mutex_writecount;
static my_mut_t z;                      // Makes sure there's only one reader executing a critical part of the reader's function

static my_sem_t no_reader_in_cs;        // Blocks writers till there's no reader in the critical section 
static my_sem_t writer_going_to_cs;     // Blocks readers till there the writer that came before them has finished writing

void write_database() { while(rand() > RAND_MAX/10000);  }

void read_database()  { while(rand() > RAND_MAX/10000);  }

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

/**
 * @brief Implements the writer's behaviour
 * 
 * @param arg (NULL)
 * @return (NULL)
 */
void * writer(void * arg)
{
    uint16_t count = 0;
    while (count != 640)
    {
        my_mut_lock(&mutex_writecount);
        writecount++;
        if (writecount==1)
            my_sem_wait(&writer_going_to_cs);
        my_mut_unlock(&mutex_writecount);

        my_sem_wait(&no_reader_in_cs);
        write_database();
        my_sem_post(&no_reader_in_cs);

        my_mut_lock(&mutex_writecount);
        writecount--;
        if (writecount==0)
            my_sem_post(&writer_going_to_cs);
        my_mut_unlock(&mutex_writecount);
        count++;
    }
    return (NULL);
}

/**
 * @brief Implements the reader's behaviour
 * 
 * @param arg (NULL)
 * @return    (NULL)
 */
void * reader(void * arg)
{
    uint16_t count = 0;
    while(count != 2560)
    {
        my_mut_lock(&z);
        my_sem_wait(&writer_going_to_cs);
        my_mut_lock(&mutex_readcount);       
        readcount++;
        if (readcount==1)
            my_sem_wait(&no_reader_in_cs);
        my_mut_unlock(&mutex_readcount);
        my_sem_post(&writer_going_to_cs);
        my_mut_unlock(&z);
        read_database();
        my_mut_lock(&mutex_readcount);
        readcount--;
        if (readcount==0)
            my_sem_post(&no_reader_in_cs);
        my_mut_unlock(&mutex_readcount);
        count++;
    }
    return (NULL);
}

int main(int argc, char const *argv[])
{
    uint32_t NBTHREADS = atoi(argv[1]);
    uint32_t READERS = (NBTHREADS % 2 == 0) ? NBTHREADS/2 : (NBTHREADS/2) + 1;
    uint32_t WRITERS = NBTHREADS / 2;
    pthread_t writs[WRITERS];
    pthread_t reads[READERS];
    my_mut_init(&mutex_readcount);
    my_mut_init(&mutex_writecount);
    my_mut_init(&z);
    my_sem_init(&no_reader_in_cs, 1);
    my_sem_init(&writer_going_to_cs, 1);

    int err;

    // Initiate the random generator with the process id as the seed
    srand(getpid()); 

    for (size_t i = 0; i < WRITERS; i++)
    {
        err = pthread_create(&writs[i], NULL, writer, NULL);
        if (err != 0)
            error(err, "pthread_create");
    }

    for (size_t i = 0; i < READERS; i++)
    {
        err = pthread_create(&reads[i], NULL, writer, NULL);
        if (err != 0)
            error(err, "pthread_create");
    }

    for (size_t i = 0; i < READERS; i++)
    {
        err = pthread_join(reads[i], NULL);
        if (err != 0)
            error(err, "pthread_join");
    }

    for (size_t i = 0; i < WRITERS; i++)
    {
        err = pthread_join(writs[i], NULL);
        if (err != 0)
            error(err, "pthread_join");
    }
    
    my_sem_destroy(&no_reader_in_cs);
    my_sem_destroy(&writer_going_to_cs);
    my_mut_destroy(&mutex_readcount);
    my_mut_destroy(&mutex_writecount);
    my_mut_destroy(&z);
    
    return 0;
}
