#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define MASTER 0      /* rank of first task */
#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

#define posicao(I, J, COLUNAS) ((I) * (COLUNAS) + (J))

int printMatrix(int rows, int cols, float *a);
int readMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename);  //Função para preencher uma matriz com os dados recebidos de um arquivo.
int writeMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename); //Função para preencher um arquivo com os dados recebidos de uma matriz.

int y;
int w;
int v;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv); // Inicializar MPI

    int quantProcs, rank, numWorkers, fonte, dest, mtype, rc;

    MPI_Comm_size(MPI_COMM_WORLD, &quantProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Variáveis de controle de envio e recebimento de matrizez
    int auxAverow, auxExtra, auxOffset, auxRows;
    int aAverow, aExtra, aOffset, aRows;
    int dAverow, dExtra, dOffset, dRows;

    MPI_Status status;

    // Variáveis para receber valores do argv
    y = atoi(argv[1]);
    w = atoi(argv[2]);
    v = atoi(argv[3]);
    char arqA[100];
    char arqB[100];
    char arqC[100];
    char arqD[100];
    strcpy(arqA, argv[4]);
    strcpy(arqB, argv[5]);
    strcpy(arqC, argv[6]);
    strcpy(arqD, argv[7]);

    // Criaçao // e alocação // das matrizes // em uma etapa
    float *matrizA; // = (float *)malloc(y * w * sizeof(float));
    float *matrizB; // = (float *)malloc(w * v * sizeof(float));
    float *matrizC; // = (float *)malloc(v * 1 * sizeof(float));
    float *matrizD; // = (float *)malloc(y * 1 * sizeof(float));
    float *aux;     // = (float *)malloc(y * v * sizeof(float));

    // Declaração de variáveis que serão usadas para a multiplicação e redução
    int i;
    int j;
    int k;

    double soma = 0.0;
    double somaT = 0.0;

    numWorkers = quantProcs - 1;

    /**************************** master task ************************************/
    if (rank == MASTER)
    {
        // Criaçao e alocação das matrizes em uma etapa
        matrizA = (float *)malloc(y * w * sizeof(float));
        matrizB = (float *)malloc(w * v * sizeof(float));
        matrizC = (float *)malloc(v * 1 * sizeof(float));
        matrizD = (float *)malloc(y * 1 * sizeof(float));
        aux = (float *)malloc(y * v * sizeof(float));

        // Ler os valores dos dados arquivos pelo ArgV para suas respectivas matrizes
        readMatrix(y, w, matrizA, arqA);
        readMatrix(w, v, matrizB, arqB);
        readMatrix(v, 1, matrizC, arqC);

        double startMpi = MPI_Wtime();

        // Variáveis para o controle de tempo
        double time_spent = 0.0;

        struct timeval start, end;

        // Inicia a contagem de tempo da região paralela
        gettimeofday(&start, NULL);

        // A B sender
        aAverow = y / numWorkers;
        aExtra = y % numWorkers;
        aOffset = 0;

        mtype = FROM_MASTER;
        for (dest = 1; dest <= numWorkers; dest++)
        {
            aRows = (dest <= aExtra) ? aAverow + 1 : aAverow;
            MPI_Send(&aRows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrizA[aOffset], aRows * w, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(matrizB, w * v, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);

            aOffset = aOffset + aRows * w;
        }

        free(matrizA);
        free(matrizB);

        // aux receiver
        auxAverow = y / numWorkers;
        auxExtra = y % numWorkers;
        auxOffset = 0;

        mtype = FROM_WORKER;
        for (i = 1; i <= numWorkers; i++)
        {
            auxRows = (i <= auxExtra) ? auxAverow + 1 : auxAverow;
            fonte = i;
            MPI_Recv(&aux[auxOffset], auxRows * v, MPI_FLOAT, fonte, mtype, MPI_COMM_WORLD, &status);

            auxOffset = auxOffset + auxRows * v;
        }

        // aux C sender
        auxAverow = y / numWorkers;
        auxExtra = y % numWorkers;
        auxOffset = 0;

        mtype = FROM_MASTER;
        for (dest = 1; dest <= numWorkers; dest++)
        {
            auxRows = (dest <= auxExtra) ? auxAverow + 1 : auxAverow;
            MPI_Send(&auxRows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&aux[auxOffset], auxRows * v, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(matrizC, v * 1, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);

            auxOffset = auxOffset + auxRows * v;
        }

        free(matrizC);
        free(aux);

        // D receiver
        dAverow = y / numWorkers;
        dExtra = y % numWorkers;
        dOffset = 0;

        mtype = FROM_WORKER;
        for (i = 1; i <= numWorkers; i++)
        {
            dRows = (i <= dExtra) ? dAverow + 1 : dAverow;
            fonte = i;
            MPI_Recv(&matrizD[dOffset], dRows * 1, MPI_FLOAT, fonte, mtype, MPI_COMM_WORLD, &status);

            dOffset = dOffset + dRows * 1;
        }

        // D sender
        dAverow = y / numWorkers;
        dExtra = y % numWorkers;
        dOffset = 0;

        mtype = FROM_MASTER;
        for (dest = 1; dest <= numWorkers; dest++)
        {
            dRows = (dest <= dExtra) ? dAverow + 1 : dAverow;
            MPI_Send(&dRows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrizD[dOffset], dRows * 1, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);

            dOffset = dOffset + dRows * 1;
        }

        // Reduc
        somaT = 0.0;
        mtype = FROM_WORKER;
        for (i = 1; i <= numWorkers; i++)
        {
            fonte = i;
            MPI_Recv(&soma, 1, MPI_DOUBLE, fonte, mtype, MPI_COMM_WORLD, &status);
            somaT += soma;
        }

        //  Fim da contagem do tempo
        gettimeofday(&end, NULL);

        // Cálculo do tempo de execução
        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

        // Printa o tempo de execução da regiao paralela
        printf("Time elpased is %d seconds and %d micros\n", seconds, micros);

        double finish = MPI_Wtime();
        printf("Done in %f seconds.\n", finish - startMpi);

        writeMatrix(y, 1, matrizD, arqD);
        free(matrizD);

        printf("%.2f\n", somaT);
    }

    /**************************** worker task ************************************/
    if (rank > MASTER)
    {
        // Alocação das matrizes em uma etapa
        matrizA = (float *)malloc(y * w * sizeof(float));
        matrizB = (float *)malloc(w * v * sizeof(float));
        matrizC = (float *)malloc(v * 1 * sizeof(float));
        matrizD = (float *)malloc(y * 1 * sizeof(float));
        aux = (float *)malloc(y * v * sizeof(float));

        // A * B = aux
        mtype = FROM_MASTER;
        MPI_Recv(&aRows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(matrizA, aRows * w, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(matrizB, w * v, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (k = 0; k < v; k++)
        {
            for (i = 0; i < aRows; i++)
            {
                aux[posicao(i, k, v)] = 0.0;
                for (j = 0; j < w; j++)
                {
                    aux[i * v + k] = aux[i * v + k] + matrizA[i * w + j] * matrizB[j * v + k];
                }
            }
        }

        // send aux to master
        mtype = FROM_WORKER;
        MPI_Send(aux, aRows * v, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD);

        free(matrizA);
        free(matrizB);

        // aux * C = D
        mtype = FROM_MASTER;
        MPI_Recv(&auxRows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(aux, auxRows * v, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(matrizC, v * 1, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (k = 0; k < 1; k++)
        {
            for (i = 0; i < auxRows; i++)
            {
                matrizD[posicao(i, k, 1)] = 0.0;
                for (j = 0; j < v; j++)
                {
                    matrizD[i * 1 + k] = matrizD[i * 1 + k] + aux[i * v + j] * matrizC[j * 1 + k];
                }
            }
        }

        // send D to master
        mtype = FROM_WORKER;
        MPI_Send(matrizD, aRows * 1, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD);

        free(matrizC);
        free(aux);

        // reduc D
        mtype = FROM_MASTER;
        MPI_Recv(&dRows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(matrizD, dRows * 1, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (i = 0; i < dRows; i++)
        {
            for (j = 0; j < 1; j++)
            {
                soma += matrizD[i * 1 + j];
            }
        }

        // send D to master
        mtype = FROM_WORKER;
        MPI_Send(&soma, 1, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);

        free(matrizD);
    }

    MPI_Finalize();
    return 0;
}

int readMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename)
{
    FILE *pf;
    pf = fopen(filename, "r");
    if (pf == NULL)
        return 0;

    register unsigned int i, j;

    for (i = 0; i < rows; ++i)
    {
        for (j = 0; j < cols; ++j)
        {
            fscanf(pf, "%f", &a[i * cols + j]);
        }
    }
    fclose(pf);
    return 1;
}

int writeMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename)
{
    FILE *pf;
    pf = fopen(filename, "w");

    if (pf == NULL)
        return 0;

    register unsigned int i, j;

    for (i = 0; i < rows; ++i)
    {
        for (j = 0; j < cols; ++j)
        {
            fprintf(pf, "%.2f\n", a[i * cols + j]);
        }
    }
    fclose(pf);
    return 1;
}