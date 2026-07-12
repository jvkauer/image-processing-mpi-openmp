# Processamento Híbrido de Imagens (MPI + OpenMP)

Este repositório contém a implementação de um filtro de imagens (conversão para escala de cinza) utilizando paralelismo híbrido com C, MPI e OpenMP, desenvolvido como trabalho final acadêmico.

## Arquitetura da Solução

O principal desafio técnico do projeto foi a execução em um cluster onde os nós não compartilham o mesmo sistema de arquivos. Para solucionar o gargalo de I/O e evitar falhas de leitura, foi adotada a arquitetura (Master-Worker):

1. **I/O Centralizado:** O Processo Mestre (Rank 0) é o único responsável por realizar a leitura do arquivo original em disco e alocar a estrutura de dados.
2. **Distribuição (MPI):** Utilizou-se a rotina coletiva `MPI_Scatter` para particionar e distribuir a matriz de pixels de forma equitativa entre os processos instanciados na rede.
3. **Processamento Multithread (OpenMP):** Cada nó recebe sua carga de dados e utiliza a diretiva `#pragma omp parallel for` para fatiar o processamento do filtro entre as threads do processador local.
4. **Reagrupamento:** Os dados processados são remontados no Mestre através do `MPI_Gather`, que realiza a escrita do arquivo de saída final no disco.

## Processamento do Filtro (Escala de Cinza)

A aplicação do filtro de escala de cinza foi escolhida por ser um problema classificado como **embaraçosamente paralelo** (*embarrassingly parallel*). Isso significa que cada pixel da imagem é processado de forma totalmente independente de seus vizinhos, eliminando a necessidade de comunicação de fronteira (troca de *ghost cells*) entre os processos MPI durante a etapa de cálculo.

O processamento aritmético local em cada thread do OpenMP consiste em extrair a média de luminosidade dos canais de cor e reatribuí-la para formar um tom neutro:
```c
int media = (pixel.r + pixel.g + pixel.b) / 3;
pixel.r = media;
pixel.g = media;
pixel.b = media;
```

## Tecnologias e Formato
- **Linguagem:** C
- **Bibliotecas:** `mpi.h` e `omp.h`
- **Formato de Imagem:** PPM Binário (P6). A escolha deste formato *raw* se deu para permitir a manipulação direta de memória sem a necessidade de lincar bibliotecas externas de compressão (como libpng) no cluster.

## Instruções de Uso

A imagem de entrada deve obrigatoriamente estar no formato PPM (P6), ser nomeada como `ufpel.ppm` e possuir uma quantidade total de pixels divisível pelo número de processos solicitados ao MPI.

### Compilação
O repositório inclui um `Makefile` padrão. No terminal, execute:
```bash
make
```

### Execução
Para disparar o processamento (exemplo utilizando 4 processos):
```bash
mpirun -n 4 ./exec
```

Ao término da execução, o sistema gerará automaticamente o arquivo `saida.ppm` no mesmo diretório, contendo a imagem com o filtro aplicado.
