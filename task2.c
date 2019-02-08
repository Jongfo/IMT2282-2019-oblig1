#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>     /* printf */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* fork */
#define SHARED 1

sem_t sem[6];     /* one semaphore for each thread */

struct threadargs {
  int id;         /* thread number */
  int sec;        /* how many seconds to sleep */
  int signal[6];  /* which threads to signal when done */
};


/* thread function: start by waiting on my own semaphore if it has been
 * initialized to 0; do my work (sleeping); signal the threads that
 * should start when I finish; exit
*/
void* tfunc(void *arg) {
  int i;
  struct threadargs *targs=arg;
  sem_wait(&sem[targs->id-1]);
  printf("Tråd %d kjører\n", targs->id);
  sleep(targs->sec);
  printf("Tråd %d er ferdig og vekker kanskje andre...\n", targs->id);
  for(i=0;i<6;i++) {
    if(targs->signal[i]==1) { sem_post(&sem[i]); }
  }
  pthread_exit(NULL);
}


int main(void)
{
  pthread_t tid[6];
  struct threadargs *targs[6];

  // allocate memory for threadargs and zero out semaphore signals
  for(int i=0;i<6;i++) { 
    targs[i] = (struct threadargs*) malloc(sizeof(struct threadargs));
    for(int j=0;j<6;j++) { targs[i]->signal[j]=0; }
  }

  targs[0]->id=1;             // thread number 1
  targs[0]->sec=1;            // how long to sleep
  targs[0]->signal[1]=1;      // which threads to wake up when done
  targs[0]->signal[4]=1;
  sem_init(&sem[targs[0]->id-1],SHARED,1); // start waiting(0) or running(1)?
  pthread_create(&tid[0], NULL, tfunc, (void *) targs[0]);

  targs[1]->id=2;             // thread number 1 
  targs[1]->sec=2;            // how long to sleep 
  targs[1]->signal[3]=1;      // which threads to wake up when done 
  sem_init(&sem[targs[1]->id-1],SHARED,0); // start waiting(0) or running(1)?
  pthread_create(&tid[1], NULL, tfunc, (void *) targs[1]);

  targs[2]->id=3;             // thread number 1 
  targs[2]->sec=3;            // how long to sleep 
  targs[2]->signal[3]=1;      // which threads to wake up when done 
  sem_init(&sem[targs[2]->id-1],SHARED,1); // start waiting(0) or running(1)?
  pthread_create(&tid[2], NULL, tfunc, (void *) targs[2]);

  targs[3]->id=4;             // thread number 1 
  targs[3]->sec=2;            // how long to sleep 
  sem_init(&sem[targs[3]->id-1],SHARED,0); // start waiting(0) or running(1)?
  pthread_create(&tid[3], NULL, tfunc, (void *) targs[3]);

  targs[4]->id=5;             // thread number 1 
  targs[4]->sec=3;            // how long to sleep 
  targs[4]->signal[5]=1;      // which threads to wake up when done 
  sem_init(&sem[targs[4]->id-1],SHARED,0); // start waiting(0) or running(1)?
  pthread_create(&tid[4], NULL, tfunc, (void *) targs[4]);

  targs[5]->id=6;             // thread number 1 
  targs[5]->sec=3;            // how long to sleep 
  sem_init(&sem[targs[5]->id-1],SHARED,0); // start waiting(0) or running(1)?
  pthread_create(&tid[5], NULL, tfunc, (void *) targs[5]);


  for(int i=0;i<=5;i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}