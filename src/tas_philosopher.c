/**************
 * 
 * file : activewait_philosophe.c
 * 
 * This code was inspired by the philosophers problem implementation done during 
 * the lectures of LINFO1252 - Operating systems. Except that instead of using
 * POSIX synchronisation primitive types we are going to use our implementation of active 
 * wait.
 * 
 ***************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "test_and_set.h"
#include "philosopher.h"

static uint64_t MAX = 100000;
static uint64_t PHILOSOPHERS;
static uint64_t NB_BAGUETTES;

void pense(){}

void mange(){}

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
 * @brief Implements each philosopher's behaviour.
 * 
 * @param arg (philo_args *) : pointer to the philosopher's argument structure.
 * @return void* : NULL.
 */
void * philosophe(void * arg)
{
    philo_args * args = (philo_args *) arg;
    uint64_t left = args->id;
    uint64_t right= (left+1) % NB_BAGUETTES;
    baguette * baguettes = args->baguettes;
    uint64_t i = 0;

    while (i < MAX)
    {
        pense();
        if (left < right)
        {
            my_mut_lock(&baguettes[left]);
            my_mut_lock(&baguettes[right]);
        } else 
        {
            my_mut_lock(&baguettes[right]);
            my_mut_lock(&baguettes[left]);
        }
        mange();
        my_mut_unlock(&baguettes[left]);
        my_mut_unlock(&baguettes[right]);
        i++;
    }
    return(NULL);
}

int main(int argc, char const *argv[])
{
    PHILOSOPHERS = strtol(argv[1], NULL, 10);
    NB_BAGUETTES = (PHILOSOPHERS > 1) ? PHILOSOPHERS : 2;   // We need atleast two baguettes
    baguette baguettes[NB_BAGUETTES];
    pthread_t phils[PHILOSOPHERS];
    philo_args arguments[PHILOSOPHERS];
    int err;

    for (size_t i = 0; i < NB_BAGUETTES; i++)
    {
        my_mut_init(&baguettes[i]);
    }

    for (size_t i = 0; i < PHILOSOPHERS; i++)
    {
    
        arguments[i] = (philo_args) {&baguettes[0], i};
        err = pthread_create(&phils[i], NULL, philosophe, (void *)&(arguments[i]));
        if (err != 0)
            error(err, "pthread_create");
    }
    
    for (size_t i = 0; i < PHILOSOPHERS; i++)
    {
        err = pthread_join(phils[i], NULL);
        if (err != 0)
            error(err, "pthread_join");
    }

    for (size_t i = 0; i < PHILOSOPHERS; i++)
    {
        my_mut_destroy(&baguettes[i]);
    }
    return (EXIT_SUCCESS);
}
