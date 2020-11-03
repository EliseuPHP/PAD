#include <omp.h> // Cabecalho para o OpenMP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int printMatrix(int rows, int cols, float *a);
int readmatrix(unsigned int rows, unsigned int cols, float *a, const char *filename);

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

  readmatrix(y, w, matrizA, arqA);
  readmatrix(w, v, matrizB, arqB);
  readmatrix(v, 1, matrizC, arqC);


  printf("%s\n\n",arqA);
  printMatrix(y, w, matrizA);
  printf("%s\n\n",arqB);
  printMatrix(w, v, matrizB);
  printf("%s\n\n",arqC);
  printMatrix(v, 1, matrizC);

// Inicio de uma regiao paralela
#pragma omp parallel
  {
    printf("Ola mundo... do thread = %d\n",
           omp_get_thread_num());
  }
  // Fim da regiao paralela
}

int readmatrix(unsigned int rows, unsigned int cols, float *a, const char *filename)
{
  // printf("\nentrou\n");

  FILE *pf;
  pf = fopen(filename, "r");
  if (pf == NULL)
    return 0;

  register unsigned int i, j;
  char k[15];

  for (i = 0; i < rows; ++i)
  {
    for (j = 0; j < cols; ++j)
    {
      fscanf(pf, "%s", k);
      // printf("%s\t", k);
      a[i * cols + j] = strtof (k, NULL);
    }
  }
  // printf("\npassou\n");
  fclose(pf);
  return 1;
}

int printMatrix(int rows, int cols, float *a)
{
  register unsigned int i, j;

  for (i = 0; i < rows; i++)
  {
    for (j = 0; j < cols; j++)
    {
      printf("%.2f\t", a[i * cols + j]);
    }
    printf("\n");
  }
  return 1;
}