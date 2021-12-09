/*******************
 * 
 * file : test_and_test_and_set.h
 * 
 * This header contains the definitions of he lock function
 * that implements the test-and-test-and-set mechanism lock. 
 * The other functions are the as the test-and-set mechanism. 
 * That's why we import "test_and_set.h"
 * 
 *******************/
#ifndef TEST_AND_TEST_AND_SET
#define TEST_AND_TEST_AND_SET

#include <stdint.h>

// We choose to represent a mutex by an integer
typedef int my_mut_t;

void my_mut_init(my_mut_t *);
void my_mut_lock(my_mut_t * );
void my_mut_unlock(my_mut_t * );
void my_mut_destroy(my_mut_t * );

#endif //TEST_AND_TEST_AND_SET
