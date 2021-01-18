//
// Created by 李若昊 on 11/3/20.
//

#include "mpi.h"
#include <cstdio>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int myrank, nprocs, name_len, flag;
    double start_time, end_time;    //开始时间戳和结束时间戳
    char host_name[20];

    MPI_Initialized(&flag);
    fprintf(stderr, "flag:%d/n", flag);

    MPI_Init(nullptr, nullptr);

    MPI_Comm_rank(MPI_COMM_WORLD, &myrank); //各MPI进程
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); //相应进程组中有多少进程

    MPI_Get_processor_name(host_name, &name_len);

    if (myrank == 0) {
        fprintf(stderr, "Precision of MPI_WTIME(): %f.\n", MPI_Wtick());
        fprintf(stderr, "Host Name:%s\n", host_name);
    }

    start_time = MPI_Wtime();
    sleep(myrank * 3);
    end_time = MPI_Wtime();

    fprintf(stderr, "myrank: %d. I have slept %f seconds.\n", myrank, end_time - start_time);

    MPI_Finalize();

    return 0;
}
