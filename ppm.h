#ifndef PPM_H
#define PPM_H

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int width, height, max_color;
    Pixel *data;
} Image;

Image ler_imagem(const char *filename);
void salvar_imagem(const char *filename, Image img);

#endif