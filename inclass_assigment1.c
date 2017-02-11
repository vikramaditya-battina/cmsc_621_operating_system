/*
   Assume that mmapped file has four numbers a, b, c, and sum = a + b + c
   Three process are executed 
   First process increments a
   Second process increments b
   Third  process increment c
   Each time one of the happends sum is updated
   Add proper mutual exlusion to ensure that sum is always equal a+b+c

  Before executing this command
  dd if=/dev/zero of=mmapped_file.txt  bs=1024 count=1024
*/
#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<string.h>
#include<semaphore.h>
#define OFF_SET 0
void print_values(int *a, int *b, int *c, int *sum){
   printf("Sum is %d, a is %d, b is %d, c is %d", *sum, *a, *b, *c);
}
int main(int argc, char *argv[]){
  int fd = open("mmapped_file.txt", O_RDWR);
  char *option;
  sem_t *semp;
  int i, *a, *b, *c;
  if(fd < 0){
     fprintf(stderr,"Unable to open mmmaped file, please check permissions or whether file is created");
     exit(1);
  }
  int *addr = (int *)mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, OFF_SET);
  if(addr == (void *)-1){
     fprintf(stderr,"Failed in creating mmap");
     return 0; 
  }
  if(argc <= 1){
     fprintf(stderr, "Please provide which process to start by giving 1 or 2 or 3 in command line arguments");
     exit(1);
  }
  printf("I am good");
  semp = sem_open("my_sem",O_CREAT, 0666, 1);
  if( semp == (void *)-1 ){
     fprintf(stderr, "Semphore initialization failed :)");
     exit(1);
  }
  option = argv[1];
  int *sum = addr+3;
  a = addr;
  b = addr + 1;
  c = addr + 2;
  printf("address is %u\n",*addr);
  if(strcmp(option, "1")==0){
     a = addr;
     for(i = 0 ; i < 10; i++){
         sleep(1);
         (*a)++;
        sem_wait(semp);
        (*sum)++;
        sem_post(semp);
     }
     print_values(a, b, c, sum);
  }else if(strcmp(option, "2") == 0){
    for(i = 0; i < 10; i++){
       (*b)++;
       sleep(1);
       sem_wait(semp);
       (*sum)++;
       sem_post(semp);
    }
    print_values(a, b, c, sum);
  }else if(strcmp(option, "3") == 0){
    for(i = 0 ; i < 10; i++){
      (*c)++;
      sleep(1);
      sem_wait(semp);
      (*sum)++;
      sem_post(semp);
    }
    print_values(a, b, c, sum);
  }else{
    fprintf(stderr, "Please provide valid option");
    exit(1); 
  }
  return 0;
}
