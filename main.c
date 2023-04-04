#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define BUFFER_SIZE 5  // buffer size
#define PRO_NO 1       // producers number
#define COM_NO 2       // consumers number
#define COUNT_TO 1000  // number to stop counting

/* User -defined data type for shared variables */
int n;
int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int count = 0;
// empty: used to count the number of empty slots in the buffer
// full: used to count the number of full slots
// mutex: binary semaphore used to protect the number of empty slots in the
//  buffer
sem_t empty, full, mutex;

void *Producer(void *arg) {
    /* producer code */
    do {
        sem_wait(&empty);
        sem_wait(&mutex);

        // critical section
        buffer[in] = count;
        printf("\033[0;34mProducer %d:\033[0m\n", *((int *) arg));
        printf("\t\033[0;33mPosted value: %d\033[0m\n", count);
        printf("\t\033[0;33mAt position: %d\033[0m\n", in);
        count = count + 1;
        in = (in + 1) % BUFFER_SIZE;
        sleep(1);
        // critical section

        sem_post(&mutex);
        sem_post(&full);
    } while (count < COUNT_TO);
}

void *Consumer(void *arg) {
    /* consumer code */
    do {
        sem_wait(&full);
        sem_wait(&mutex);

        // critical section
        int captured = buffer[out];
        printf("\033[0;34mConsumer %d:\033[0m\n", *((int *) arg));
        printf("\t\033[0;32mCaptured value: %d\033[0m\n", captured);
        printf("\t\033[0;33mAt position: %d\033[0m\n", out);
        out = (out + 1) % BUFFER_SIZE;
        sleep(1);
        // critical section

        sem_post(&mutex);
        sem_post(&empty);
        //consumer the item in nextConsumed
    } while(count < COUNT_TO);
}

int main(int argc, char* argv[]) {
    /* destroy any semaphores still on the heap */
    sem_destroy(&full);
    sem_destroy(&empty);
    sem_destroy(&mutex);

    /* create and initialize semaphores */
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 0);

    int a[5] = {1,2,3,4,5};
    /* create multiple producer threads */
    pthread_t pro[PRO_NO];
    for (int idx = 0; idx < PRO_NO; idx++) {
        pthread_create(&pro[idx], NULL, (void *)Producer, (void *) &a[idx]);
    }

    /* create mutliple conusmer threads */
    pthread_t com[COM_NO];
    for (int idx = 0; idx < COM_NO; idx++) {
        pthread_create(&com[idx], NULL, (void *)Consumer, (void *)&a[idx]);
    }
    pthread_exit(NULL); // terminalte calling thread
    return 0;
}
