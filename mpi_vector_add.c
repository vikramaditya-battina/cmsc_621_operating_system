/*
 For this problem , you will implement parallel addition using MPI. Assume large Array of integers and your goal is to add all the elements and return the sum. Instead of using sequential procedure to do this, we could use 5 process to do parallel addition. The master process will divide the array into 4 equal parts and pas thr four parts to the processes 1-5 using MPI_Send each process would perform the addition on their cunk of the arrayu and return back the sum to the process0. Process 0 will aggregate the sum and geenerate the sum of the entire array.
compile-->> 
    mpicc -o <outputfilename> <inputfilename>
run-->>
    mpirun -v -np 5 <outputfilename> 
*/
#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#define MASTER_RANK  0
#define NUM_ELEMENTS  113
#define TAG  99
int master(int num_process){
  int *arr = (int *)malloc(sizeof(int) * NUM_ELEMENTS);
  for(int i = 0 ; i < NUM_ELEMENTS; i++){
    arr[i] = i;
  }
  int indi_size = NUM_ELEMENTS / (num_process-1) ;
  int *curr = arr;
  for(int dest = 1; dest < num_process; dest++){
      if(dest == num_process-1){
        MPI_Send(curr, indi_size+(NUM_ELEMENTS%(num_process-1)), MPI_INT, dest, TAG, MPI_COMM_WORLD);
      }else{
        MPI_Send(curr, indi_size, MPI_INT, dest, TAG, MPI_COMM_WORLD);
      }
      curr = curr + indi_size;
  }
  int final_sum = 0;
  fprintf(stderr,"\nmaster sent values to clients\n");
  for(int dest = 1; dest < num_process; dest++){
     int curr_sum;
     MPI_Recv(&curr_sum, 1, MPI_INT, dest, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
     final_sum = final_sum + curr_sum;
  }
  fprintf(stderr, "\nMaster final value is %d\n", final_sum);
  MPI_Finalize();
  return final_sum;
}
void slave(int rank,int num_process){
   int indi_size = NUM_ELEMENTS / (num_process-1);
   if(rank == num_process-1){
      indi_size = indi_size + (NUM_ELEMENTS%(num_process-1));
   }
   int *arr = (int *)malloc(sizeof(int) * indi_size);
   MPI_Recv(arr, indi_size, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   fprintf(stderr, "\nslave recived and about to start computing..\n");
   int sum = 0;
   for(int i = 0 ; i <  indi_size; i++){
     sum = sum + arr[i];
   }
   MPI_Send(&sum, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
   fprintf(stderr, "slave computed values to %d", sum);
   MPI_Finalize();
}
int main(int argc, char **argv){
  int rank, num_process;
  MPI_Init(NULL, NULL);
  if( MPI_Comm_rank(MPI_COMM_WORLD, &rank) != 0){
    fprintf(stderr, "\nError while fetching the rank \n");
    exit(1);
  }
  fprintf(stderr, "%d process started\n", rank);
  if(MPI_Comm_size(MPI_COMM_WORLD, &num_process)){
     fprintf(stderr, "\nError while fetching the number of process\n");
     exit(1);
  }
  if(rank == MASTER_RANK){
     int val = master(num_process);
     fprintf(stderr, "\nThe final sum of the Vector is %d\n", val);
  }else{
     slave(rank, num_process);
  }
  return 0;
}
