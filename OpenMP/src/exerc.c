#include <omp.h> // Cabecalho para o OpenMP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int y;
int w;
int v;

int main(int argc, char *argv[])
{
    // pega variaveis do argv
    y = atoi(argv[1]);
    w = atoi(argv[2]);
    v = atoi(argv[3]);
    char arqA[10];
    char arqB[10];
    char arqC[10];
    char arqD[10];
    strcpy(arqA, argv[4]);
    strcpy(arqB, argv[5]);
    strcpy(arqC, argv[6]);
    strcpy(arqD, argv[7]);

    float *matrizA = (float *)malloc(y * w * sizeof(float));
    float *matrizB = (float *)malloc(w * v * sizeof(float));
    float *matrizC = (float *)malloc(v * 1 * sizeof(float));
    float *matrizD = (float *)malloc(y * 1 * sizeof(float));

    readFile(arqC);

// Inicio de uma regiao paralela
#pragma omp parallel
    {
        printf("Ola mundo... do thread = %d\n",
               omp_get_thread_num());
    }
    // Fim da regiao paralela
}

int readFile(const char *filename)
{
    char c[1000];
    FILE *fptr;
    if ((fptr = fopen(filename, "r")) == NULL)
    {
        printf("Error! opening file");
        // Program exits if file pointer returns NULL.
        exit(1);
    }

    // reads text until newline is encountered
    fscanf(fptr, "%[^\n]", c);
    printf("Data from the file:\n%s", c);
    fclose(fptr);

    return 0;
}

int printMatrix(int rows, int cols, float **a)
{
    int row, columns;
    for (row = 0; row < rows; row++)
    {
        for (columns = 0; columns < cols; columns++)
        {
            printf("%f     ", a[row][columns]);
        }
        printf("\n");
    }
    return 1;
}