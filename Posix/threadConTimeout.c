/**
 * 
 * TRACCIA DEL 16-01-2024
 * 
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define TIMEOUT 1 /* secondi o microsecondi? */

pthread_t thread;
pthread_mutex_t lock;
int threadTerminato = 0;

void* doSomething() {
    usleep(TIMEOUT*2);

    printf("doSomething()..\n");

    pthread_mutex_lock(&lock);
    threadTerminato = 1;
    pthread_mutex_unlock(&lock);

    return NULL;
}

void doInTheMeantime() { 
    printf("doInTheMeantime()..\n");
}

void startAsynch() {
    pthread_mutex_init(&lock, NULL);
    pthread_create(&thread, NULL, &doSomething, NULL);
}

void waitEnd(int t) {
    usleep(t);
    pthread_mutex_lock(&lock);
    if(threadTerminato) { 
        /* il thread ha terminato per tempo */
        printf("[ status: THREAD OK ]\n");
        pthread_join(thread, NULL);
    } else { 
        /* il thread non ha terminato per tempo */
        printf("[ status: THREAD KILLED ]\n");
        pthread_cancel(thread);
    }
    pthread_mutex_unlock(&lock);
}

int main(int argc, char* argv[]) {

    startAsynch();
    doInTheMeantime();
    waitEnd(TIMEOUT);

    return 0;
}
