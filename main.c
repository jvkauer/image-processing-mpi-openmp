#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "ppm.h" // Importa a biblioteca que criei para os arquivos imagem ppm

int imageProcessing(int worldSize, int myRank){

    int width, height;
    Image img;

    if (myRank == 0){
        img = ler_imagem("ufpel.ppm");
        width = img.width;
        height = img.height;
    }

    // Envia para todos os nós as informações de altura e largura por meio do broadcast
    MPI_Bcast( &height, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast( &width, 1, MPI_INT, 0, MPI_COMM_WORLD );

    printf("Rank %d: A imagem lida tem %d de largura e %d de altura.\n", myRank, width, height);

    if (myRank != 0){
        printf("Eu sou o Worker %d e ouvi que a imagem tem %d x %d!\n", myRank, width, height);
    }

    int pixels_por_processo = (width * height) / worldSize;
    Pixel *meu_pedaco = (Pixel*) malloc(pixels_por_processo * sizeof(Pixel));

    // MPI_Scatter pra dividir a nossa imagem em partes iguais e distribuila
    MPI_Scatter(img.data, pixels_por_processo * 3, MPI_UNSIGNED_CHAR, 
            meu_pedaco, pixels_por_processo * 3, MPI_UNSIGNED_CHAR, 
            0, MPI_COMM_WORLD);

    #pragma omp parallel for
    for (int i = 0; i < pixels_por_processo; i++) {
        int media = (meu_pedaco[i].r + meu_pedaco[i].g + meu_pedaco[i].b) / 3;
        meu_pedaco[i].r = media;
        meu_pedaco[i].g = media;
        meu_pedaco[i].b = media;
    }

    // Todos enviam as fatias prontas de volta para o Mestre (Gather)
    MPI_Gather(meu_pedaco, pixels_por_processo * 3, MPI_UNSIGNED_CHAR, 
               img.data, pixels_por_processo * 3, MPI_UNSIGNED_CHAR, 
               0, MPI_COMM_WORLD);

    //  Apenas o mestre grava o resultdo final no disco
    if (myRank == 0){
        salvar_imagem("saida.ppm", img);
        // Libera a imagem grandona
        free(img.data);
    }

    // Todo mundo limpa a memória
    free(meu_pedaco);

    return 0;
}

int main(int argc, char** argv){

    int worldSize, myRank;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    imageProcessing(worldSize, myRank);

    MPI_Finalize();
    
    return 0;
}