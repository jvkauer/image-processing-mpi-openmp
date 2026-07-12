// ppm.c
#include <stdio.h>
#include <stdlib.h>
#include "ppm.h" // Importa suas structs!

void skip_comments(FILE *fp) {
    int ch;
    while (1) {
        // O espaço em branco no fscanf faz ele pular todas as quebras 
        // de linha, espaços e tabs automaticamente!
        fscanf(fp, " "); 
        
        ch = fgetc(fp); // Pega o próximo caractere útil
        
        if (ch == '#') {
            // Se for um '#', joga tudo fora até o final da linha
            while ((ch = fgetc(fp)) != '\n' && ch != EOF);
        } else {
            // Se for um número (como o 960), devolve ele pro arquivo e sai
            ungetc(ch, fp);
            break;
        }
    }
}

Image ler_imagem(const char *filename) {
    Image img;
    FILE *fp = fopen(filename, "rb"); // "rb" = read binary
    
    if (!fp) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }
    char formato[3];
    fscanf(fp, "%2s", formato);
    
    if (formato[0] != 'P' || formato[1] != '6') {
        printf("Formato de imagem invalido! (Precisa ser P6)\n");
        exit(1);
    }
    skip_comments(fp);
    fscanf(fp, "%d %d", &img.width, &img.height);
    
    skip_comments(fp);
    fscanf(fp, "%d", &img.max_color);
    
    // Pula o único caractere de quebra de linha após o max_color
    fgetc(fp); 

    // Calcula o total de pixels e aloca memória
    int total_pixels = img.width * img.height;
    img.data = (Pixel*) malloc(total_pixels * sizeof(Pixel));

    // Parâmetros: onde salvar, tamanho de cada item, quantos itens, qual arquivo
    fread(img.data, sizeof(Pixel), total_pixels, fp);
    fclose(fp);
    return img;
}

void salvar_imagem(const char *filename, Image img) {
    FILE *fp = fopen(filename, "wb"); // "wb" = write binary
    
    if (!fp) {
        printf("Erro ao criar o arquivo %s\n", filename);
        exit(1);
    }

    // Escreve o cabeçalho em texto puro
    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", img.width, img.height);
    fprintf(fp, "%d\n", img.max_color);

    // Escreve todos os pixels em binário de uma vez só
    int total_pixels = img.width * img.height;
    fwrite(img.data, sizeof(Pixel), total_pixels, fp);
    fclose(fp);
}