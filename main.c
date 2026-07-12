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
        unsigned char original_r = meu_pedaco[i].r;
        unsigned char original_g = meu_pedaco[i].g;
        unsigned char original_b = meu_pedaco[i].b;

        // Fórmula matemática padrão do Sépia
        int tr = (int)(0.393 * original_r + 0.769 * original_g + 0.189 * original_b);
        int tg = (int)(0.349 * original_r + 0.686 * original_g + 0.168 * original_b);
        int tb = (int)(0.272 * original_r + 0.534 * original_g + 0.131 * original_b);

        // O valor não pode passar de 255
        meu_pedaco[i].r = (tr > 255) ? 255 : tr;
        meu_pedaco[i].g = (tg > 255) ? 255 : tg;
        meu_pedaco[i].b = (tb > 255) ? 255 : tb;
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

    int worldSize, myRank, aux, dest;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    imageProcessing(worldSize, myRank);

    MPI_Finalize();
    
    return 0;
}