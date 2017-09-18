/*
 * par_sum.c
 *
 * CS 470 Project 1 (Pthreads)
 * Parallel version
 *
 * Compile with --std=c99
 *
 * Authors: John Unger and David Thompson
 *
 * Date: January 29, 2016
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Global variable accessed and edited during runtime.
volatile int waiting_threads = 0; // Thread counter

// Setup and initialize our mutexes and conidition.
pthread_mutex_t wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t math_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Global aggregate variables.
long sum = 0, odd = 0;
long min = INT_MAX;
long max = INT_MIN;
bool done = false;

// Linked list acting as task queue.
typedef struct node {
	long val;
	struct node * next;
} node_t;
// Node positions for beginning and end of task queue.
node_t * front = NULL;
node_t * rear = NULL;

// Function prototype.
void update(long number);

/* Each pthreads responsibility.*/
void* thread_work()
{
	long dur;
	
	while (!done) {
		pthread_mutex_lock(&wait_mutex);
		waiting_threads++; // Thread is accounted for.
		
		// Thread is waiting to reopen.
		while (pthread_cond_wait(&wait_cond, &wait_mutex) != 0);  
		//Set temp to the front node in the linked list.
		node_t * temp = front;
		
		// If list is empty flag.
		if (front == NULL) {
			dur = -1;
		}
		// If only one link is in the list.
		else if (front == rear) {
			dur = front->val; //take value from head
			pthread_mutex_lock(&list_mutex);
			front = rear = NULL; //remove node
			pthread_mutex_unlock(&list_mutex);

		}
		else {
			dur = front->val; //take value from head
			pthread_mutex_lock(&list_mutex);
			front = front->next; //move head forward
			pthread_mutex_unlock(&list_mutex);
		}
		free(temp); //free temp malloc
		waiting_threads--; //decrement thread counter
		pthread_mutex_unlock(&wait_mutex);

		update(dur); 
	}
	return NULL;
}

/* Update global aggregate variables given a duration. */
void update(long number)
{
	int sl = (int) number; //placeholder
	
	// Complete global computations if a valid duration.
	if (sl != -1) {
		sleep(sl);
		printf("\tThread worked for %d second(s).\n", sl);
		
		// Update global aggregate variables.
		pthread_mutex_lock(&math_mutex);
		sum += number;
		if (number % 2 == 1) {
			odd++;
		}
		if (number < min) {
			min = number;
		}
		if (number > max) {
			max = number;
		}
		pthread_mutex_unlock(&math_mutex);
	}
}

/* Function of the master thread and main process, which creates and assigns
     work to each thread. */
int main(int argc, char* argv[])
{
	// Check and parse command line options.
	if (argc != 3) {
		printf("Usage: <infile> <num-thread>\n");
		exit(EXIT_FAILURE);
	}
	
	char *fn = argv[1]; //job file
	long nthreads = strtol(argv[2], NULL, 10); //number of threads
	
	// Allocates and returns a pointer to a place in memory.
	pthread_t* threads = (pthread_t*) malloc( sizeof(pthread_t) * nthreads);
	
	//Create all the threads requested.
	for (int i = 0; i < nthreads; i++) {
		pthread_create(&threads[i], NULL, thread_work, NULL);
	}

	// Wait for all threads to hit the barrier.
	while(waiting_threads < nthreads);

	// Load numbers and add them to the task queue.
	FILE* fin = fopen(fn, "r");
	char action;
	long num;
	while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
		//Checks number is valid.
		if (num < 0) {
			printf("Can not work for a negative amount of time.\n"); 
		}

		// Worker process
		else if (action == 'p') {	
			// Create memory allocation for our linked list.
			node_t * temp = (node_t *) malloc(sizeof(node_t));
			temp->next = NULL;
			temp->val = num;

			pthread_mutex_lock(&list_mutex);

			// If task queue is empty.
			if (front == NULL && rear == NULL) {
				front = rear = temp;
			}
			//Else create a new node.
			rear->next = temp;
			rear = temp;

			pthread_mutex_unlock(&list_mutex);

			// Wake an idle worker.
			do {
				pthread_cond_signal(&wait_cond);
			} while (waiting_threads > 0 && front != NULL);

		}
		// Master thread process.
		else if (action == 'w') {
			// main wait
			int sl = (int) num;
			// Simulate computation.
			printf("\nMaster waiting %d second(s).\n", sl);
			sleep(sl);
			printf("Master awakens!\n");
		}
		else {
			printf("ERROR: Unrecognized action: '%c'\n", action);
			exit(EXIT_FAILURE);
		}

	}

	fclose(fin); // Close file input.

	// Finish the remaining queue.
	do {
		pthread_cond_signal(&wait_cond);
	} while (front != NULL);	

	while (waiting_threads < nthreads);
	
	done = !done; // global flag
	
	// Wake all threads available for work.
	pthread_cond_broadcast(&wait_cond);



	// Kill worker threads.
	for (int t = 0; t < nthreads; t++) {
		pthread_join(threads[t], NULL);
	}
	
	// Print result totals and thread demise.
	printf("Work has finished, all threads are dead.\n");
	printf("\nResults: Total: %ld Odds: %ld Min: %ld Max: %ld.\nGoodbye world.\n", sum, odd, min, max);

	// Destroy mutexes and condition.
	pthread_mutex_destroy(&math_mutex);
	pthread_mutex_destroy(&list_mutex);
	pthread_mutex_destroy(&wait_mutex);
	pthread_cond_destroy(&wait_cond);
	// Free pthreads from malloc and exit.
	free(threads);
	return (EXIT_SUCCESS);
}
