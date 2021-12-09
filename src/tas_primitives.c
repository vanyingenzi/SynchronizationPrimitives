/***********
 * 
 * file : test_and_set.c
 * 
 * This file implements the test-and-set lock mechanism using inline assembly.
 * 
 ***********/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "test_and_set.h"
#include "my_semaphore.h"

/**
 * @brief Initiliasize the mutex variable refered by the "mutex" argument. 
 * 
 * @param mutex (my_mut_t *) :  A pointer to the variable 
 *                              that's going to be used fo synchronization
 * 
 * @remark The function doesn't return nothing cause the only source of failure
 *         is a segmentation fault and handling process signals won't be useful for 
 *         our case
 */
void my_mut_init(my_mut_t * mutex)
{
    *mutex = 0;
}

/**
 * @brief The variable referenced by the "mutex" (my_mut_t *) shall be locked by the call 
 * of this function. Implemented using test-and-set.
 * 
 * @remark : 
 * - The variable should be initiated with my_mut_init prior, otherwise the behaviour is 
 * unpredictable.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 * @param mutex (my_mut_t *) : A pointer to the (my_mut_t) mutex variable.
 */
void my_mut_lock(my_mut_t * mutex)
{
    int past_value;
    do {
        asm("movl $1, %%eax\n\t"
            "xchgl %1, %%eax\n\t"
            "movl %%eax, %0"
            : "=m" (past_value)
            : "m" (*mutex)
            : "eax" );
    } while(past_value != 0);
}

/**
 * @brief The variable referenced by the "mutex" (my_mut_t *) shall be unlocked by the call 
 * of this function. 
 * 
 * @remark : 
 * - The variable should be initiated with my_mut_init otherwise the behaviour is 
 * unpredictable.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 * @param mutex (my_mut_t *) : A pointer to the (my_mut_t) mutex variable.
 */
void my_mut_unlock(my_mut_t * mutex)
{
    asm("movl $0, %%eax\n\t"
        "xchgl %%eax, %0\n\t"
        : "=m" (*mutex)
        : /*No input operands*/
        : "eax" );
}

/**
 * @brief The variable referenced by the "mutex" (my_mut_t *) shall be destroyed by the call 
 * of this function. A destroyed "mutex" can be reinitiated with a call to my_mut_init. 
 *  
 * @remark : 
 * - The variable should be initiated with my_mut_init otherwise the behaviour is 
 * unpredictable.
 * - The behaviour of a destroyed mutex that hasn't been reinitiated is unpredictable.
 * - The function doesn't return nothing cause the only source of failure
 * is a segmentation fault and handling process signals won't be useful for our case.
 * 
 * @param mutex (my_mut_t *) : A pointer to the (my_mut_t) mutex variable.
 */
void my_mut_destroy(my_mut_t * mutex)
{
    *mutex = rand();
}