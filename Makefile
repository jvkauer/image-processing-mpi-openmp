# Compilador
CC = mpicc

# Flags de compilação (ativando o OpenMP)
CFLAGS = -fopenmp -Wall

# Nome do executável final
TARGET = trabalho_final

all: $(TARGET)

$(TARGET): main.c ppm.c
	$(CC) $(CFLAGS) main.c ppm.c -o $(TARGET)

clean:
	rm -f $(TARGET) saida.ppm