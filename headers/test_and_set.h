/*******************
 * 
 * file : test_and_set.h
 * 
 * This header contains the structures and definitions of functions
 * used in our implementation of test-and-set mechanism lock.
 * 
 *******************/
#ifndef TEST_AND_SET
#define TEST_AND_SET

#include <stdint.h>

// We choose to represent a mutex by an integer
typedef int my_mut_t;

void my_mut_init(my_mut_t *);
void my_mut_lock(my_mut_t * );
void my_mut_unlock(my_mut_t * );
void my_mut_destroy(my_mut_t * );

#endif //TEST_AND_SET
