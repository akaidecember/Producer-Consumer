/*
Assignment 3 (CSC 139 OS)
Process Synchronization
@Anshul Kumar

This program implements the solution to the bounded-buffer problem using the producer and consumer process.
*/

#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<pthread.h>
#include "buffer.h"

static unsigned int seed = 1;

//This structure consists of the variables for handling the buffer
//This structure also contains the semaphores and the mutex variables for easy access
//Added structure for data control
typedef struct{
	
    bufferItem buffer[BUFFER_SIZE];
    int in, out, count;									//Has the insert value, remove value and the count of threads
    sem_t *full, *empty;								//semaphores
    pthread_mutex_t *mutex;								//mutex
	
} threadBuffer;											//Using thread buffer 

//Function to initialize the above structure (threadBuffer)
threadBuffer* initializeBuffer(){
	
	//allocating memory for hte tempBuffer
    threadBuffer *tempBuffer = (threadBuffer *)malloc(sizeof(threadBuffer));
	
	//this case will run if the mem.alloc. did not go as planned
    if (tempBuffer == NULL)
        return NULL;
	
	//else, the initialization will take place
    tempBuffer -> in = 0;
    tempBuffer -> out = 0;
    tempBuffer -> count = 0;
    tempBuffer -> empty = (sem_t *)malloc(sizeof(sem_t));
    sem_init(tempBuffer -> empty, 0, BUFFER_SIZE);
    tempBuffer -> full = (sem_t *)malloc(sizeof(sem_t));
    sem_init(tempBuffer -> full, 0, 0);
    tempBuffer -> mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(tempBuffer -> mutex, NULL);
    return tempBuffer;
	
}

int insert_item(bufferItem item, threadBuffer *buffer)
{
	
    int retVal = -1;									//Default return value if process somehow fails
    sem_wait(buffer -> empty);							//Acquire the semaphore
    pthread_mutex_lock(buffer -> mutex);				//Acquire the mutex lock
    
    if (buffer -> count < BUFFER_SIZE) {
		
        buffer -> buffer[buffer -> in] = item;			//put the item at in
        buffer -> in++;
        buffer -> in %= BUFFER_SIZE;				
        buffer -> count++;
        retVal = 0;										//set the return value to 0 since success
        printf("Producer produced %d\n", item);
		
    }

    pthread_mutex_unlock(buffer -> mutex);				//Release the lock
    sem_post(buffer -> full);							//Release the semaphore
    return retVal;
	
}

void *producer(void *tempBuffer){
	
    threadBuffer *buffer;
    bufferItem item;
    pthread_t tid;
    unsigned int now;									//Variable for the random function

    buffer = (threadBuffer *)tempBuffer;				//Casting the tempBuffer since it is type void by default
    now = time(NULL);									
    tid = pthread_self();								//Get the ID of this.thread
    seed = now^tid;					

    while (1) {
		
        sleep(rand_r(&seed) % 6 + 1);					//random value
        item = rand_r(&seed);							//set item to the above ransom value
        if (insert_item(item, buffer))
            printf("Error in inserting item\n");
		
    }
	
    return;
	
}

int remove_item(bufferItem *item, threadBuffer *buffer){
	
    int retVal = -1;									//Default return value if process somehow fails	
    sem_wait(buffer -> full);							//acquire the semaphore
    pthread_mutex_lock(buffer -> mutex);				//Acquire the lock

    if (buffer -> count > 0) {
		
        *item = buffer -> buffer[buffer -> out];		//Get the item stored at out		
        buffer -> out++;
        buffer -> out %= BUFFER_SIZE;
        buffer -> count--;
        retVal = 0;										//set to 0 since success
        printf("consumer  consumed %d\n", *item);
		
    }

    pthread_mutex_unlock(buffer -> mutex);				//releasing the lock
    sem_post(buffer -> empty);							//Releasing the semaphore
    return retVal;
	
}

void *consumer(void *tempBuffer){
	
    threadBuffer *buffer;
    bufferItem item;
    buffer = (threadBuffer *)tempBuffer;				//Casting the tempBuffer to the type threadBuffer
    while (1){
		
        sleep(rand_r(&seed)% 6 + 1);					//random value sleep
        if (remove_item(&item, buffer))
            printf("Error in removal of item\n");
		
    }
	
    return;
	
}

//Main function of the program
int main(int argc, char **argv){
	
    threadBuffer *buffer;
    int sleepTime, producers, consumers, threads;
    int i = 0;

	//Exit if insufficient arguments are given
	if(argc < 4){
			
			printf("Invalid number of arguments!\n");
			exit(0);
		
	}

	//Get the arguments from the command line and set them to the variables
    sleepTime = atoi(argv[1]);
	producers = strtol(argv[2], NULL, 0);
    consumers = strtol(argv[3], NULL, 0);
    threads = producers + consumers;							//total
    pthread_t tid[threads];										//array of threads
	
	//Initialize the above created structure UD buffer
    buffer = initializeBuffer();

	//Create the threads in the above declared tid array for both producers and consumers
    for (i = 0; i < consumers; i++)
        if (pthread_create(&tid[i], NULL, consumer, buffer))
            fprintf(stderr, "Could not create thread\n");
    for (i = 0; i < producers; i++) 
        if (pthread_create(&tid[i], NULL, producer, buffer))
            fprintf(stderr, "Could not create thread\n");
   
	//sleep this.thread for sleepTime number of "sleepTime" seconds
    sleep(sleepTime);
    return 0;
	
}
