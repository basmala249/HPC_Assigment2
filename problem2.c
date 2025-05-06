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

    int *g = (int*) malloc(numofP * sizeof(int));
    int *chunks = (int*) malloc(numofP * sizeof(int));
    int *displacements = (int*) malloc(numofP * sizeof(int));

    int step = n / numofP;
    int extra = n % numofP;
    int local_step = 0;

    if (rank == 0) {
        int offset = 0;
        for (int i = 0; i < numofP; i++) {
            // Master takes extra rows 
            if (i == 0 && extra > 0) {
                g[i] = step + extra;
            } else {
                g[i] = step;
            }
            chunks[i] = g[i] * m;
            displacements[i] = offset;
            offset += chunks[i];
        }
    }
    MPI_Scatter(g, 1, MPI_INT, &local_step, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *A = NULL, *B = NULL, *C = NULL;
    int *tempA = NULL, *tempB = NULL, *tempC = NULL;
    if (rank == 0) {
        A = (int*) malloc(size * sizeof(int));
        B = (int*) malloc(size * sizeof(int));
        C = (int*) malloc(size * sizeof(int));
    }
    tempA = (int*) malloc(local_step * m * sizeof(int));
    tempB = (int*) malloc(local_step * m * sizeof(int));
    tempC = (int*) malloc(local_step * m * sizeof(int));

    if (rank == 0) {
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

    MPI_Scatterv(A, chunks, displacements, MPI_INT, tempA, local_step * m, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(B, chunks, displacements, MPI_INT, tempB, local_step * m, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < (local_step * m); i++) {
        tempC[i] = tempA[i] + tempB[i];
    }

    MPI_Gatherv(tempC, local_step * m, MPI_INT, C, chunks, displacements, MPI_INT, 0, MPI_COMM_WORLD);

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
    free(g);
    free(chunks);
    free(displacements);

    MPI_Finalize();
    return 0;
}
