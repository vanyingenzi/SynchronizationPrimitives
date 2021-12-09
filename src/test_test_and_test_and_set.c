/***************
 * test_test_and_test_and_set.c
 * 
 * This program is going to run the given number of thread and synchronize them 
 * using an implemenation of a "active-wait" mutex that is implemented in the 
 * test_and_test_and_set.c file.
 * 
 ****************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_and_test_and_set.h"

int WORK; // The number of iteration each thread have to do 

my_mut_t lck; // The mutex 

void process(){ while(rand() > RAND_MAX/10000); }

/**
 * @brief The behavious of a each thread
 * 
 * @param arg (NULL)
 * @return    (NULL) 
 */
void * thread(void * arg)
{
    int count = 0;
    while (count < WORK)
    {
        my_mut_lock(&lck);
        process();
        my_mut_unlock(&lck);
        count++;
    }
    return(NULL);
}

int main(int argc, char const *argv[])
{
    int N = atoi(argv[1]);
    pthread_t THREADS[N];
    WORK = 6400/N;
    my_mut_init(&lck);
    
    for (size_t i = 0; i < N; i++)
    {
        pthread_create(&THREADS[i], NULL, thread, NULL);
    }

    for (size_t i = 0; i < N; i++)
    {
        pthread_join(THREADS[i], NULL);
    }
    my_mut_destroy(&lck);
    return 0;
}
