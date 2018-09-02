#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

#define NUM 8192

int main(void){
    int comm_sz;
    int my_rank;

    double local_start, local_finish, local_elapsed, elapsed;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int* A;

    int* local_A;
    int local_n = NUM * NUM / comm_sz;
    local_A = (int*)malloc(sizeof(int)*local_n);

    int* local_x;
    local_x = (int*)malloc(sizeof(int) * NUM);

    int* local_y;
    local_y = (int*)malloc(sizeof(int)*NUM/comm_sz);

    //printf("comm_sz %d my_rank: %d\n", comm_sz, my_rank);

    if(my_rank == 0){
        A = (int*)malloc(sizeof(int)*NUM*NUM);
        int i;

        for(i = 0; i < NUM * NUM; i++){
            A[i] = i % 1000;
        }
        for(i = 0; i < NUM; i++){
            local_x[i] = i % 1000;
        }

        MPI_Barrier(MPI_COMM_WORLD);
        local_start = MPI_Wtime();

        MPI_Scatter(A, local_n, MPI_INT, local_A, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(local_x, NUM, MPI_INT, 0, MPI_COMM_WORLD);
    }else{
        MPI_Barrier(MPI_COMM_WORLD);
        local_start = MPI_Wtime();

        MPI_Scatter(A, local_n, MPI_INT, local_A, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(local_x, NUM, MPI_INT, 0, MPI_COMM_WORLD);

    }

    int j, l;
    for(j = 0; j < NUM / comm_sz; j++){
        local_y[j] = 0;
        for(l = 0; l < NUM; l++){
            local_y[j] += local_A[NUM/comm_sz * j + l] * local_x[l];
        }
    }

    int* y = NULL;
    if(my_rank == 0){
        y = malloc(sizeof(int) * NUM);
        MPI_Gather(local_y, NUM/comm_sz, MPI_INT, y, NUM/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
        local_finish = MPI_Wtime();
        local_elapsed = local_finish - local_start;
    }else{
        MPI_Gather(local_y, NUM/comm_sz, MPI_INT, y, NUM/comm_sz, MPI_INT, 0, MPI_COMM_WORLD);
        local_finish = MPI_Wtime();
        local_elapsed = local_finish - local_start;
    }

    MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(my_rank == 0){
        printf("%f secondes\n", elapsed*1000);
    }

    return 0;
}
