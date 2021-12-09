/***************
 * 
 * file : philosophe.h
 * 
 * This file contains all the structures used in the philosophers 
 * problem implementation.
 * 
 ***************/

#ifndef PHILOSOPHER
#define PHILOSOPHER

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#if defined(TEST_AND_TEST_AND_SET) || defined(TEST_AND_SET) // If we imported our primitives
// Renamed the my_mut_t -> baguettes
typedef my_mut_t baguette;
#else
// Renamed the mutexes -> baguettes
typedef pthread_mutex_t baguette;
#endif

/**
 * @brief The structure to be passed as the argument to the philosophers function.
 * 
 * @param baguettes (baguette *) : The array of baguettes, in other words array of mutex.
 * @param id        (uint64_t)   : The identity of the philospher which indicates him, which baguettes he can
 *                                  have access to.
 */
typedef struct 
{
    baguette * baguettes;
    uint64_t id;
}philo_args;

#endif //PHILOSOPHER