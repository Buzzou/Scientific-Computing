#include "openmpi_test.h"
#include <mpi.h>
#include <iostream>

int main() {
    print_processors();

    // int rankID;
    // int sizeNum;
    // MPI_Init(0,0);
    // MPI_Comm_size(MPI_COMM_WORLD, &sizeNum);
    // MPI_Comm_rank(MPI_COMM_WORLD, &rankID);
    // std::cout<<"Hello world! "<<rankID<<" of total ="<<sizeNum<<std::endl;
    // MPI_Finalize();

    return 0;
}
