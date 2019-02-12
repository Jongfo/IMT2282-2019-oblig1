#include <ctype.h> //isdigit
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHARED 0  /* process-sharing if !=0, thread-sharing if =0*/
#define BUF_SIZE 20
#define MAX_MOD 100000
#define NUM_ITER 200

void *Producer(void *prodNum); /* Producer thread */
void *Consumer(void *conNum); /* Consumer thread */

pthread_barrier_t barrier3;

sem_t empty;            /* empty: How many empty buffer slots */
sem_t full;             /* full: How many full buffer slots */
sem_t b;                /* b: binary, used as a mutex */

int g_data[BUF_SIZE];   /* shared finite buffer  */
int g_idx;              /* index to next available slot in buffer, 
                           remember that globals are set to zero
                           according to C standard, so no init needed  */

int main(int argc, char *argv[]) {
	int numProdCon = 1;
	if (argc > 1){
		printf("Arguments: %d\n", argc);
		char* endp;
		numProdCon = strtol(argv[1], &endp, 10);
		if (numProdCon < 1 || numProdCon >= MAX_MOD){
			printf("Invalid argument. Pease use a numer between 1 and %d.\n", MAX_MOD);
			exit(1);
		}
	}
	
	pthread_t pid[numProdCon], cid[numProdCon];

	// Initialie the semaphores
	sem_init(&empty, SHARED, BUF_SIZE);
	sem_init(&full, SHARED, 0);
	sem_init(&b, SHARED, 1);

	// Create the threads
	printf("main started\n");
	for (int i = 0; i < numProdCon; i++){
		pthread_barrier_init(&barrier3, NULL, 2);
		pthread_create(&pid[i], NULL, Producer, &i);
		pthread_barrier_wait(&barrier3);
	}
	for (int i = 0; i < numProdCon; i++){
		pthread_barrier_init(&barrier3, NULL, 2);
		pthread_create(&cid[i], NULL, Consumer, &i);
		pthread_barrier_wait(&barrier3);
	}

	// And wait for them to finish.
	for (int i = 0; i < numProdCon; i++){
		pthread_join(pid[i], NULL);
	}
	for (int i = 0; i < numProdCon; i++){
		pthread_join(cid[i], NULL);
	}
	printf("main done\n");

	return 0;
}


void *Producer(void *prodNum) {
	int ourID = *((int*)prodNum); //cast and dereference to copy.
	pthread_barrier_wait(&barrier3);
	int i=0, j;
	while(i < NUM_ITER) {
		// pretend to generate an item by a random wait
		usleep(random()%MAX_MOD);
		
		// Wait for at least one empty slot
		sem_wait(&empty);
		// Wait for exclusive access to the buffer
		sem_wait(&b);
		
		// Check if there is content there already. If so, print 
    // a warning and exit.
		if(g_data[g_idx] == 1) { 
			printf("Producer overwrites!, idx er %d\n", g_idx); 
			exit(0); 
		}
		
		// Fill buffer with "data" (ie: 1) and increase the index.
		g_data[g_idx]=1;
		g_idx++;
		
		// Print buffer status.
		j=0; printf("(Producer%d, idx is %d): ", ourID, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exlusive access
		sem_post(&b);
		// Increase the counter of full bufferslots.
		sem_post(&full);
		
		i++;		
	}

	return 0;
}


void *Consumer(void *conNum) {
	int ourID = *((int*)conNum); //cast and dereference to copy.
	pthread_barrier_wait(&barrier3);
	int i=0, j;


	while(i < NUM_ITER) {
		// Wait a random amount of time, simulating consuming of an item.
		usleep(random()%MAX_MOD);
		
		// Wait for at least one slot to be full
		sem_wait(&full);
		// Wait for exclusive access to the buffers
		sem_wait(&b);
		
		// Check that the buffer actually contains some data 
    // at the current slot.
		if(g_data[g_idx-1] == 0) { 
			printf("Consumes nothing!, idx er %d\n",g_idx);
			exit(0);
		}
		
		// Remove the data from the buffer (ie: Set it to 0) 
		g_data[g_idx-1]=0;
		g_idx--;
		
		// Print the current buffer status
		j=0; printf("(Consumer%d, idx is %d): ", ourID, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exclusive access
		sem_post(&b);
		// Increase the counter of empty slots.
		sem_post(&empty);  	

		i++;
	}

	return 0;

}