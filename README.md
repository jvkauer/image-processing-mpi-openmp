# Processamento Híbrido de Imagens (MPI + OpenMP)

Este repositório contém a implementação de um filtro de imagens (conversão para escala de cinza) utilizando paralelismo híbrido com C, MPI e OpenMP, desenvolvido como trabalho final acadêmico.

## Arquitetura da Solução

O principal desafio técnico do projeto foi a execução em um cluster onde os nós não compartilham o mesmo sistema de arquivos. Para solucionar o gargalo de I/O e evitar falhas de leitura, foi adotada a arquitetura Mestre-Trabalhador (Master-Worker):

1. **I/O Centralizado:** O Processo Mestre (Rank 0) é o único responsável por realizar a leitura do arquivo original em disco e alocar a estrutura de dados.
2. **Distribuição (MPI):** Utilizou-se a rotina coletiva `MPI_Scatter` para particionar e distribuir a matriz de pixels de forma equitativa entre os processos instanciados na rede.
3. **Processamento Multithread (OpenMP):** Cada nó recebe sua carga de dados e utiliza a diretiva `#pragma omp parallel for` para fatiar o processamento do filtro entre as threads do processador local.
4. **Reagrupamento:** Os dados processados são remontados no Mestre através do `MPI_Gather`, que realiza a escrita do arquivo de saída final no disco.

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
mpirun -n 4 ./trabalho_final
```

Ao término da execução, o sistema gerará automaticamente o arquivo `saida.ppm` no mesmo diretório, contendo a imagem com o filtro aplicado.
