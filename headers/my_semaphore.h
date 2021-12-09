/*******************
 * 
 * file : my_semaphore.h
 * 
 * This header contains the structures and definitions of functions
 * used in our implementation of semaphores.
 * 
 * @remark :    The header works only if "test-and-set.h" xor "test-and-test-and-set"
 *              was included prior to the include of this header
 * 
 * 
 *******************/
#ifndef MY_SEMAPHORE
#define MY_SEMAPHORE

/**
 * @brief This stucture will be the de structure to represent the semaphor in
 * our implementation of a sempahorer with active-wait
 * 
 * @param mutex      (my_mut_t) : The mutex that will be used to protect the variable val.
 * @param one_access (my_mut_t) : The mutex that mutex will be used to only allow one thread to execute 
 *                                the body of the wait function.
 * @param val         (int)     : The number of threads to initially let pass.
 */
typedef struct
{
    my_mut_t mutex;
    my_mut_t one_access;
    int val;
}my_sem_t;

void my_sem_init(my_sem_t *, int);
void my_sem_wait(my_sem_t *);
void my_sem_post(my_sem_t *);
void my_sem_destroy(my_sem_t *);

#endif //MY_SEMAPHORE