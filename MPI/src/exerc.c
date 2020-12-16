#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define MASTER 0      /* rank of first task */
#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

#define posicao(I, J, COLUNAS) ((I) * (COLUNAS) + (J))

int readMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename);  //Função para preencher uma matriz com os dados recebidos de um arquivo.
int writeMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename); //Função para preencher um arquivo com os dados recebidos de uma matriz.

int y;
int w;
int v;

int main(int argc, char *argv[])
{
    int quantProcs, rank, numWorkers, fonte, dest, mtype, rows, averow, extra, offset, rc;
    // int yAverow, yExtra, yOffset,
    //     wAverow, wExtra, wOffset,
    //     vAverow, vExtra, vOffset;
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

    double a[y][w],
        b[w][v],
        c[y][v];

    // Criaçao e alocação das matrizes em uma etapa
    float *matrizA = (float *)malloc(y * w * sizeof(float));
    float *matrizB = (float *)malloc(w * v * sizeof(float));
    float *matrizC = (float *)malloc(v * 1 * sizeof(float));
    float *matrizD = (float *)malloc(y * 1 * sizeof(float));
    float *aux = (float *)malloc(y * v * sizeof(float));

    // Ler os valores dos dados arquivos pelo ArgV para suas respectivas matrizes
    readMatrix(y, w, matrizA, arqA);
    readMatrix(w, v, matrizB, arqB);
    readMatrix(v, 1, matrizC, arqC);

    // Declaração de variáveis que serão usadas para a multiplicação e redução
    int i;
    int j;
    int k;

    double soma = 0.0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &quantProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
    numWorkers = quantProcs - 1;

    /**************************** master task ************************************/
    if (rank == MASTER)
    {
        
        double start = MPI_Wtime();

        /* Send matrix data to the worker tasks */
        averow = y / numWorkers;
        extra = y % numWorkers;
        offset = 0;
        mtype = FROM_MASTER;
        for (dest = 1; dest <= numWorkers; dest++)
        {
            rows = (dest <= extra) ? averow + 1 : averow;
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            MPI_Send(&matrizA[posicao(offset, 0, w)], rows * w, MPI_FLOAT, dest, mtype,
                     MPI_COMM_WORLD);
            MPI_Send(&matrizB, w * v, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        mtype = FROM_WORKER;
        for (i = 1; i <= numWorkers; i++)
        {
            fonte = i;
            MPI_Recv(&offset, 1, MPI_INT, fonte, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, fonte, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux[posicao(offset, 0, v)], rows * v, MPI_FLOAT, fonte, mtype,
                     MPI_COMM_WORLD, &status);
        }


        printf("******************************************************\n");
        printf("Result Matrix:\n");
        for (i = 0; i < y; i++)
        {
            printf("\n");
            for (j = 0; j < v; j++)
                printf("%6.2f   ", aux[posicao(i, j, v)]);
        }
        printf("\n******************************************************\n");

        /* Measure finish time */
        double finish = MPI_Wtime();
        printf("Done in %f seconds.\n", finish - start);
    }

    /**************************** worker task ************************************/
    if (rank > MASTER)
    {
        mtype = FROM_MASTER;
        MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrizA, rows * w, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrizB, w * v, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);

        for (i = 0; i < rows; i++)
        {
            for (j = 0; j < v; j++)
            {
                aux[i * v + j] = 0.0;
                for (k = 0; k < w; k++)
                {
                    aux[i * v + j] = aux[i * v + j] + matrizA[i * w + k] * matrizB[k * v + j];
                }
            }
        }
        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
        MPI_Send(&aux, rows * v, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD);
    }
    MPI_Finalize();
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