#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>
#include <time.h>
#define BUFFER_SIZE 500
//Gustavo Dias Cavalheri Nº: 8937159

//thread para calcular o tempo remanescente
void *calcula_tempo_restante(void *x_void_ptr)
{
    //vetor com os parametros neccessarios
    int *x_ptr = (int *)x_void_ptr;

    //tempo que falta pra terminar a copia
    int remaining;

    while(!x_ptr[3]) {
        Sleep(1000); //sleep 1 seg para dar a proxima estimativa
        remaining = (x_ptr[1]*x_ptr[2])/ (x_ptr[0]); //tempo remanescente = (quantosFaltam * tempoCorrido) / (lidos)
        printf("\nTime remaining: %d",remaining);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int parametrosThread[4];
    if(argc < 1) {
        printf("Problema no numero de argumentos passados.");
        return 0;
    }
    char* arquivo = argv[2]; //nome do arquivo de origem
    char* destino = argv[3]; //nome do arquivo de destino

    parametrosThread[0] = 0; //lidos
    parametrosThread[1] = 0; //quantosFaltam
    parametrosThread[2] = 0; //tempoCorrido
    parametrosThread[3] = 0; //terminou
    int size = 0;

    //bloco que abre o arquivo
    FILE* arqEntrada = fopen(arquivo,"rb+");
    FILE* arqSaida = fopen(destino,"wb+");

    if(arqEntrada == NULL) {
        printf("Problema no arquivo passado");
        return 0;
    }

    //posiciona o arquivo no fim pra ver quantos byter tem
    fseek(arqEntrada, 0L, SEEK_END);

    //pega o tamanho do arquivo
    size = ftell(arqEntrada);
    printf("Data size of %d bytes",size);

    //bloco da thread
    pthread_t thread_tempo_restante; //thread
    unsigned char buffer[BUFFER_SIZE]; //tipo que sera lido
    size_t bytes_read = 0; //quantos bytes foram lidos
    if(pthread_create(&thread_tempo_restante, NULL, calcula_tempo_restante, parametrosThread)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    fseek(arqEntrada,0,SEEK_SET);
    clock_t begin = clock(); //pega inicio
    bytes_read = fread(&buffer, sizeof(unsigned char), BUFFER_SIZE, arqEntrada);

    //enquanto tem coisa do arquivo pra ler
    while(bytes_read > 0) {
        parametrosThread[0] += bytes_read; //total de bytes lidos
        parametrosThread[1] = size - parametrosThread[0]; //quantos bytes faltam
        parametrosThread[2] = (clock() - begin)/CLOCKS_PER_SEC; //quanto tempo foi corrido ja
        fwrite(buffer, 1, sizeof(unsigned char) * bytes_read, arqSaida);
        bytes_read = fread(buffer, sizeof(unsigned char), BUFFER_SIZE, arqEntrada);
    }
    printf("\nTotal time: %d", parametrosThread[2]); //tempo total
    parametrosThread[3] = 1; //terminou de ler o arquivo, booleano para finalizar a thread;

    return 0;
}
