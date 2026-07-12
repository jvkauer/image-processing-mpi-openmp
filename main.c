#include <stdio.h>
#include <mpi.h>

int imageProcessing(int worldSize, int myRank){

    if (myRank == 0){


    }



    return 0;
}






int main(int argc, char** argv){

    int worldSize, myRank, aux, dest;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    imageProcessing(worldSize, myRank);
    
    return 0;
}