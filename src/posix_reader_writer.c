/***********
 * 
 * file : posix_reader_writer.c
 * 
 * This code implements the reader writer problem as seen in the practical classes
 * of LINFO1252 - Operating Systems.
 * 
 ***********/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <stdint.h>

static int readcount  = 0;                      // The number of readers in the critical section 
static int writecount = 0;                      // The number of writers in the critical section 

static pthread_mutex_t mutex_readcount;         // Mutex protecting the variable readcount
static pthread_mutex_t mutex_writecount;        // Mutex protecting the variable writercount
static pthread_mutex_t z;                       // Makes sure there's only one reader executing a critical part of the reader's function

static sem_t no_reader_in_cs;                   // Blocks writers till there's no reader in the critical section 
static sem_t writer_going_to_cs;                // Blocks readers till there the writer that came before them has finished writing

void write_database() { while(rand() > RAND_MAX/10000); }

void read_database()  { while(rand() > RAND_MAX/10000); }

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
        pthread_mutex_lock(&mutex_writecount);
        writecount++;
        if (writecount==1)
            sem_wait(&writer_going_to_cs);
        pthread_mutex_unlock(&mutex_writecount);

        sem_wait(&no_reader_in_cs);
        write_database();
        sem_post(&no_reader_in_cs);

        pthread_mutex_lock(&mutex_writecount);
        writecount--;
        if (writecount==0)
            sem_post(&writer_going_to_cs);
        pthread_mutex_unlock(&mutex_writecount);
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
        pthread_mutex_lock(&z);
        sem_wait(&writer_going_to_cs);
        pthread_mutex_lock(&mutex_readcount);       
        readcount++;
        if (readcount==1)
            sem_wait(&no_reader_in_cs);
        pthread_mutex_unlock(&mutex_readcount);
        sem_post(&writer_going_to_cs);
        pthread_mutex_unlock(&z);
        read_database();
        pthread_mutex_lock(&mutex_readcount);
        readcount--;
        if (readcount==0)
            sem_post(&no_reader_in_cs);
        pthread_mutex_unlock(&mutex_readcount);
        count++;
    }
    return (NULL);
}

int main(int argc, char const *argv[])
{
    uint32_t NBTHREADS = atoi(argv[1]);
    uint32_t READERS = (NBTHREADS % 2 == 0) ? NBTHREADS/2 : (NBTHREADS/2) + 1;
    uint32_t WRITERS = NBTHREADS / 2 ;
    pthread_t writs[WRITERS];
    pthread_t reads[READERS];
    pthread_mutex_init(&mutex_readcount, NULL);
    pthread_mutex_init(&mutex_writecount, NULL);
    pthread_mutex_init(&z, NULL);
    sem_init(&no_reader_in_cs, 0, 1);
    sem_init(&writer_going_to_cs, 0, 1);

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
    
    sem_destroy(&no_reader_in_cs);
    sem_destroy(&writer_going_to_cs);
    pthread_mutex_destroy(&mutex_readcount);
    pthread_mutex_destroy(&mutex_writecount);
    pthread_mutex_destroy(&z);
    
    return 0;
}
