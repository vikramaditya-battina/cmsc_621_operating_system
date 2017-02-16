#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
void *sayHelloThreadNumber(void *args){
   int *num = (int *)args;
   fprintf(stderr, "hello from thread id %d",*num);
   //`return;
}
sem_t producer, consumer, mutex;
int len;
int start;
int end;
int *array;
void *thread_producer(void *args){
  int *thread_id = (int *)args;
  int i = 0;
  fprintf(stderr, "producer thread %d started\n", *thread_id);
  while(i < 18){
   // printf("\nchecking one thing\n");
    sem_wait(&producer);
    sem_wait(&mutex);
    array[start] = 1;
    fprintf(stderr, "producer %d produced %d\n", *thread_id, array[start]);
    start = (start +1)%len;
    sem_post(&mutex);
    sem_post(&consumer);
    i++;
  }
}
void *thread_consumer(void *args){
    int consumed_val;
    int *thread_id = (int *)args;
    int i = 0;
    fprintf(stderr, "consumer thread %d started\n", *thread_id);
    while(i < 6){
     sem_wait(&consumer);
     sem_wait(&mutex);
     consumed_val =  array[end];
     fprintf(stderr, "consumer %d consumed %d\n", *thread_id, consumed_val);
     end = (end+1)%len;
     sem_post(&mutex);
     sem_post(&producer);
     i++;
   }
}
int main(){
   pthread_t threads[4];
   int thread_ids[4];
   int i;
   void *val;
   //printf("Enter length of the buffer");
   //scanf("%d",&len);
   len = 5;
   array = (int *)malloc(sizeof(int)*len);
   //semaphore are initialising
   fprintf(stderr, "about to initialize the semaphores..\n");
   if(sem_init(&producer, 0, len) != 0){
      fprintf(stderr,"producer semaphore creation failed\n");
      exit(1);
   }
   if(sem_init(&consumer, 0, 0) != 0){
     fprintf(stderr,"consumer semaphore creation failed\n");
     exit(1);
   }
   if(sem_init(&mutex, 0, 1) != 0){
     fprintf(stderr,"mutex semphore creation failed\n");
     exit(1);
   }
   fprintf(stderr, "Creating the producer thread\n");
   start = 0;
   end = 0;
   thread_ids[0] = 0;
   pthread_create(&threads[0], NULL, thread_producer, &thread_ids[0]);
   for(i = 1; i < 4; i++){
      thread_ids[i] = i;
      fprintf(stderr,"about to create consumer thread\n");
      if( pthread_create(&threads[i],NULL,thread_consumer, &thread_ids[i]) != 0){
         printf("Consumer thread creating failed %d \n", i);   
      }
   }
   for(i=0; i < 4; i++){
      pthread_join(threads[i],&val);
   }
   return 0;
}
