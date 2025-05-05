#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <stdbool.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); 
    
    int rank , numOfProcesses ;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    
    int partitionSize , rows , cols;
    int* Matrix1 = NULL , *Matrix2 = NULL; 
    if(rank == 0){
      printf("Enter Number Of Rows :- ");
      fflush(stdout);
      scanf("%d" , &rows);
      printf("Enter Number Of Columns :- ");
      fflush(stdout);
      scanf("%d" , &cols);
      if(cols * rows < numOfProcesses){
        printf("Elements to be divided is less than No of processes.");
        fflush(stdout);
        return 0;
      }
      Matrix1 = malloc(rows * cols * sizeof(int));
      printf("Enter Elements Of Matrix1 :- ");
      fflush(stdout);
      for(int i = 0 ; i < (rows * cols) ; ++i){
        scanf("%d" , &Matrix1[i]); 
      }
      
      Matrix2 = malloc(rows * cols * sizeof(int));
      printf("Enter Elements Of Matrix2 :- ");
      fflush(stdout);
      for(int i = 0 ; i < (rows * cols) ; ++i){
        scanf("%d" , &Matrix2[i]); 
      }
      partitionSize = (rows * cols) / numOfProcesses;
    }
    MPI_Bcast(&partitionSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int* irecv1 = malloc(partitionSize * sizeof(int));
    int* irecv2 = malloc(partitionSize * sizeof(int));
    MPI_Scatter(Matrix1, partitionSize, MPI_INT, irecv1, partitionSize , MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(Matrix2, partitionSize, MPI_INT, irecv2, partitionSize , MPI_INT, 0, MPI_COMM_WORLD);
    int* result = malloc(partitionSize * sizeof(int));
    for(int i = 0 ; i < partitionSize ; i++){
      result[i] = irecv1[i] + irecv2[i];
    }
    int* values = NULL ;
    if(rank == 0){
       values = malloc(rows * cols * sizeof(int));
    }
    MPI_Gather(result, partitionSize , MPI_INT , values , partitionSize,MPI_INT, 0 ,MPI_COMM_WORLD); 
    if(rank == 0){
      for(int i =  (numOfProcesses * partitionSize) ; i < rows * cols ; ++i){
         values[i] = Matrix1[i] + Matrix2[i] ;
      }
      printf("The Final Matrix :-\n");
      fflush(stdout);
      for(int i = 0 ; i < rows * cols ; ++i){
        printf("%d " , values[i] );
        if((i + 1) % cols == 0){
          printf("\n");
          fflush(stdout);
        }
      }
      free(Matrix1);
      free(Matrix2);
      free(values);
    }
    free(irecv1);
    free(irecv2);
    free(result);
    MPI_Finalize();
    return 0;
}

