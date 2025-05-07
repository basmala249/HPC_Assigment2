#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **args) {
    MPI_Init(&argc, &args);
    int numofP;
    MPI_Comm_size(MPI_COMM_WORLD, &numofP);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int n = 0, m = 0;
    if (rank == 0) {
        scanf("%d", &n);
        scanf("%d", &m);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int size = n * m;

    int step = n / numofP; 
    int extra = n % numofP; 
    int local_rows =0;
    if(rank==0)
    local_rows = step+ extra;
    else
    local_rows = step;

    int *A = NULL, *B = NULL, *C = NULL;
    int *tempA = (int*) malloc(local_rows * m * sizeof(int));
    int *tempB = (int*) malloc(local_rows * m * sizeof(int));
    int *tempC = (int*) malloc(local_rows * m * sizeof(int));
    
    if (rank == 0) {
        A = (int*) malloc(size * sizeof(int));
        B = (int*) malloc(size * sizeof(int));
        C = (int*) malloc(size * sizeof(int));

        printf("Enter %d elements for matrix A:\n", n * m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                scanf("%d", &A[i * m + j]);
            }
        }
        printf("Enter %d elements for matrix B:\n", n * m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                scanf("%d", &B[i * m + j]);
            }
        }
    }
    
    if (numofP > 1) {
        MPI_Scatter(A, step * m, MPI_INT, tempA, step * m, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(B, step * m, MPI_INT, tempB, step * m, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        for (int i = step*m * (numofP); i < size ; i++) {
            C[i] = A[i] + B[i];
        }
    }
    
    for (int i = 0; i < local_rows * m; i++) {
        tempC[i] = tempA[i] + tempB[i];
    }
    

    if (numofP > 1) {
        MPI_Gather(tempC, step * m, MPI_INT, C, step * m, MPI_INT, 0, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("Result Matrix C:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                printf("%d ", C[i * m + j]);
            }
            printf("\n");
        }
        free(A);
        free(B);
        free(C);
    }

    free(tempA);
    free(tempB);
    free(tempC);

    MPI_Finalize();
    return 0;
}
