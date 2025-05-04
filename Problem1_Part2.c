#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <stdbool.h>

bool isPrime(int n){
  if(n <= 1)return 0;
  if(n == 2)return 1;
  if(n % 2 == 0) return 0;
  for(int i = 3 ; i * i <= n ; i += 2){
      if(n % i == 0){
        return 0;      
      }  
  }
  return 1;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    double start = MPI_Wtime();
    
    int rank , numOfProcesses ;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    
    if(rank == 0){
       if(numOfProcesses < 2){
           printf("No Avaliable Resources \n");
           fflush(stdout);
           return 0;       
       } 
       int x , r , y;
       printf("Enter Lower Bound :- ");
       fflush(stdout);
       scanf("%d" , &x);
       printf("Enter Upper Bound :- ");
       fflush(stdout);
       scanf("%d" , &y);
       r = (y - x) / (numOfProcesses - 1);
       int result = 0 ;
       for(int i = 1 ; i < numOfProcesses ; ++i){
         MPI_Send(&x , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD);
         MPI_Send(&r , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD);
         int cnt ;
         MPI_Recv(&cnt , 1 , MPI_INT , i  , 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
         result += cnt;
       }
       for(int i = x + (numOfProcesses - 1) * r ; i <= y ; ++i){
          if(isPrime(i)){
             result++;
          }      
      }
      printf("Total No Of Primes = %d.\n" , result);  
      
      double end = MPI_Wtime();
      printf("Total time: %f seconds\n", end - start);
    }
    else {
       int cnt = 0 ;
       int x , r;
       MPI_Recv(&x , 1 , MPI_INT , 0 , 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
       MPI_Recv(&r , 1 , MPI_INT , 0 , 0, MPI_COMM_WORLD , MPI_STATUS_IGNORE);
       int a = x + (rank - 1) * r;
       int b = x + rank * r - 1;
       for(int i = a ; i <= b ; ++i){
          if(isPrime(i))cnt++;       
       }
       MPI_Send(&cnt , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
    } 
    MPI_Finalize();
    return 0;
}

