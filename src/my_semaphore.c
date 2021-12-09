/*******************
 * 
 * file : my_semaphore.c
 * 
 * Implements our own version of semaphore using the primitives that
 * implement the test-and-test-and-set mechanism.
 * 
 *******************/
#include <stdio.h>
#include <stdlib.h>

#ifdef TEST_AND_TEST_AND_SET
#include "test_and_test_and_set.h"
#else
#include "test_and_set.h"
#endif
#include "my_semaphore.h"

/**
 * @brief Initialize the semaphore structure and all it's attributes
 * 
 * @param sem (my_sem_t *) : A pointer to the semaphore to initiate.
 * @param val (int)        : The initial value of the semaphore.
 * 
 * @remark: 
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 */
void my_sem_init(my_sem_t * sem, int val)
{
    my_mut_init(&(sem->mutex));
    my_mut_init(&(sem->one_access));
    sem->val = val;
} 

/**
 * @brief Implements the semaphore waiting mechanism. If the value of the semaphore
 * is zero then the thread waits till another thread executes post on the same 
 * semaphore.
 * 
 * @param sem (my_sem_t *) : A pointer to the semaphore to wait on.
 * 
 * @remark: 
 * - The semaphore pointed should be initiated by the my_sem_init function.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 */
void my_sem_wait(my_sem_t * sem)
{
    // Block new threads if someone is already blocked
    my_mut_lock(&(sem->one_access));
    my_mut_lock(&(sem->mutex));
    if(sem->val <= 0)
    {
        my_mut_unlock(&(sem->mutex));
        while (sem->val <= 0);  // Only leaves someone does a post cause
        my_mut_lock(&(sem->mutex));
        sem->val = sem->val-1;
        my_mut_unlock(&(sem->mutex));
        my_mut_unlock(&(sem->one_access));
    }else 
    {
        sem->val = sem->val-1;
        my_mut_unlock(&(sem->mutex));
        my_mut_unlock(&(sem->one_access));
    }
}

/**
 * @brief Implements the post signaling of a semaphore. It increments the value of the 
 * semaphore.
 * 
 * @param sem (my_sem_t *) : A pointer to the semaphore to increment the value.
 * 
 * @remark: 
 * - The semaphore pointed should be initiated by the my_sem_init function.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 */
void my_sem_post(my_sem_t * sem)
{
    my_mut_lock(&(sem->mutex));
    sem->val = sem->val + 1;
    my_mut_unlock(&(sem->mutex));
}

/**
 * @brief Destroys the semaphore pointed by the sem argument and it's parameters. and sets the value of 
 * the semaphore to a random variable.
 * 
 * @param sem (my_sem_t *) : A pointer to the semaphore to increment the value.
 * 
 * @remark: 
 * - The semaphore pointed should be initiated by the my_sem_init function.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 */
void my_sem_destroy(my_sem_t * sem)
{
    my_mut_destroy(&(sem->mutex));
    my_mut_destroy(&(sem->one_access));
    sem->val = rand();
}