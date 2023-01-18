/* 
CPSC/ECE 3220, Spring 2021
Roderick "Rance" White
Program 2

The following program is the implementation of five pthread functions to initialize, enter, exit, and finalize a region, as well as a predicate function that is meant to return 1. 
*/


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include "region.h"


void init_region( region_t *r, int max )
{
	r->signature = 0xc1c0ffee;						//Make signature an 'unlikely' hex constant.
	r->count = 0;									//Initialized to 0 because empty.
	r->max_count = max;								//Equal to max value given.
	pthread_mutex_init(&(r->lock), NULL);
	pthread_cond_init(&(r->delay_cv), NULL);
	r->inside_region_list = (pthread_t*) calloc(r->max_count, sizeof(pthread_t));
}

void enter_region( region_t *r, int (* predicate)() )
{
	int i, j = -1;
	// Check the signature
	if(r->signature != 0xc1c0ffee) {
		printf("Thread trying to enter an uninitialized region.\n");
		pthread_exit(NULL);
	}

	pthread_mutex_lock(&(r->lock));						//Obtain mutex

	//Thread must wait on condition variable if count is at max or predicate returns false.
	while(r->count >= r->max_count || predicate() != 1)
		pthread_cond_wait(&(r->delay_cv), &(r->lock));	//Waits on condition variable

	//Loop to check if thread is already in list
	for(i=0; i<r->max_count; i++) {
		//Set j to the index of the first empty entry.
		if(j==-1 && r->inside_region_list[i] == 0) j=i;

		//Function will return 0 if not equal
		if(pthread_equal(pthread_self(), r->inside_region_list[i]) != 0) {
			printf("Thread calling enter_region() was not inside region.\n");
			pthread_mutex_unlock(&(r->lock));			//Release the mutex
			pthread_exit(NULL);
		}			
	}
	r->inside_region_list[j] = pthread_self();			//Place new thread into the first empty entry
	r->count = r->count + 1;							//Increment count
	pthread_mutex_unlock(&(r->lock));					//Release the mutex
}

void exit_region( region_t *r )
{
	int i;
	// Check the signature
	if(r->signature != 0xc1c0ffee) {
		printf("Thread trying to exit an uninitialized region.\n");
		pthread_exit(NULL);
	}

	pthread_mutex_lock(&(r->lock));					//Obtain mutex

	//Loop to check if the calling thread is in list
	for(i=0; i<r->max_count; i++) {
		//Change entry to 0 and return if the thread is present. 
		if(pthread_equal(pthread_self(),r->inside_region_list[i])!=0) {
			r->inside_region_list[i] = 0;			//Replace thread with 0
			r->count = r->count - 1;				//Decrement count
			pthread_cond_broadcast(&(r->delay_cv));	//Wake up all threads on condition signal
			pthread_mutex_unlock(&(r->lock));		//Release the mutex
			return;									//Return
		}			
	}

	//If this point is reached, the calling thread wasn't in the list
	printf("Thread calling exit_region() was already inside region.\n");
	pthread_mutex_unlock(&(r->lock));				//Release the mutex
	pthread_exit(NULL);
}


void finalize_region( region_t *r )
{
	// Check the signature
	if(r->signature != 0xc1c0ffee) printf("Thread trying to finalize an uninitialized region.\n");

	//Destroy the mutex and condition variable in the region
	pthread_mutex_destroy(&(r->lock));
	pthread_cond_destroy(&(r->delay_cv));
	free(r->inside_region_list);

	// Error if the count in the region is not zero
	if(r->count != 0) printf("%d threads left inside a region.\n",r->count);
}

int true_predicate()
{
	return 1;
}


