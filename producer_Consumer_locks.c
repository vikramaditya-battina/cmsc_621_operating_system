#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
int len = 0;
int front = 0;
int back = 0;
int size;
int *arr;
struct params{
  int len;
  int size;
  int front;
  int back;
  int *arr;
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_variable1 = PTHREAD_COND_INITIALIZER, condition_variable2 = PTHREAD_COND_INITIALIZER;

void *producer(void *args){
  int *thread_id = (int *)args;
  int i  = 0;
  fprintf(stderr,"producer thread %d started\n",*thread_id);
  while( i < 9999){
    pthread_mutex_lock(&mutex);
    while(size ==  len){
       pthread_cond_wait(&condition_variable1, &mutex);
    }
    fprintf(stderr,"The producer %d produced %d\n",*thread_id,i+1);
    arr[front] = i+1;
    front = (front+1)%len;
    size = size+1;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&condition_variable2);
    //pthread_mutex_unlock(&mutex);
    i++;
  }
}

void *consumer(void *args){
  int *thread_id = (int *)args;
  int i = 0;
  fprintf(stderr,"consumer thread %d started\n", *thread_id);
  while(i < 3333){
     pthread_mutex_lock(&mutex);
     while(size == 0){
       pthread_cond_wait(&condition_variable2, &mutex);
     }
     int consumed_val = arr[back];
     fprintf(stderr, "consumer %d consumed %d\n", *thread_id, consumed_val);
     back  = (back+1)%len;
     size--;
     pthread_mutex_unlock(&mutex);
     pthread_cond_signal(&condition_variable1);
     //pthread_mutex_unlock(&mutex);
     i++;
  }
}

int main(){
  len = 5;
  size = 0;
  pthread_t threads[4];
  struct params p;
  p.len = 5;
  p.size = 0;
  p.front = 0;
  p.back = 0;
  p.arr = (int *)malloc(sizeof(int) * len);
  int thread_ids[4] = {0, 1, 2, 3};
  void *retval;
  arr = (int *)malloc(sizeof(int)*len);
  if( pthread_create(&threads[0],NULL,producer,&thread_ids[0]) != 0){
    fprintf(stderr,"producer thread creation failed\n");
    exit(1);
  }
  for(int i = 1; i < 4; i++){
     if(pthread_create(&threads[i], NULL, consumer, &thread_ids[i]) != 0){
        fprintf(stderr,"consumer thread %d creation failed\n",thread_ids[i]);
     }
  }
  for(int i = 0 ; i < 4; i++){
    pthread_join(threads[i],&retval);
  }
  return 0;
}
