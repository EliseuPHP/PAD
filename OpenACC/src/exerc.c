#include <openacc.h> // Cabecalho para o OpenACC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int readMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename);  //Função para preencher uma matriz com os dados recebidos de um arquivo.
int writeMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename); //Função para preencher um arquivo com os dados recebidos de uma matriz.

int y;
int w;
int v;

int main(int argc, char *argv[])
{
  // Variáveis para receber valores do argv
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

  // Declaração de variáveis que serão usadas para a multiplicação
  int i;
  int j;
  int k;

  double soma = 0.0;

  // Variáveis para o controle de tempo
  double time_spent = 0.0;

  struct timeval start, end;

  // Inicia a contagem de tempo da região paralela
  gettimeofday(&start, NULL);

  // Inicio de uma regiao paralela
#pragma acc data copy(matrizA, matrizB, matrizC, aux) copyout(matrizD)
  {
    // A*B = aux
#pragma acc parallel
    {
#pragma acc loop collapse(3)
      for (i = 0; i < y; i++)
      {
        for (j = 0; j < v; j++)
        {
          // aux[i * v + j] = 0.0;
          for (k = 0; k < w; k++)
          {
            aux[i * v + j] = aux[i * v + j] + matrizA[i * w + k] * matrizB[k * v + j];
          }
        }
      }
    }

    // aux*C = D
#pragma acc parallel
    {
#pragma acc loop collapse(3)
      for (i = 0; i < y; i++)
      {
        for (j = 0; j < 1; j++)
        {
          // matrizD[i * 1 + j] = 0.0;
          for (k = 0; k < v; k++)
          {
            matrizD[i * 1 + j] = matrizD[i * 1 + j] + aux[i * v + k] * matrizC[k * 1 + j];
          }
        }
      }
    }
  }
  // Fim da regiao paralela

  // Desaloca matrizes já utilizadas
  free(matrizA);
  free(matrizB);
  free(matrizC);
  free(aux);

  // Redução da matrizD por soma
#pragma acc parallel loop collapse(2) reduction(+ \
                                                : soma)
  for (i = 0; i < y; i++)
  {
    for (j = 0; j < 1; j++)
    {
      soma += matrizD[i * 1 + j];
    }
  }

  //  Fim da contagem do tempo
  gettimeofday(&end, NULL);

  // Cálculo do tempo de execução
  long seconds = (end.tv_sec - start.tv_sec);
  long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

  // Printa o tempo de execução da regiao paralela
  // printf("Time elpased is %d seconds and %d micros\n", seconds, micros);

  // Escreve matriz D no arquivo
  writeMatrix(y, 1, matrizD, arqD);

  // Desaloca matriz já utilizada
  free(matrizD);

  // Printa o resultado da redução da matriz D
  printf("%.2f\n", soma);
}

int readMatrix(unsigned int rows, unsigned int cols, float *a, const char *filename)
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
      a[i * cols + j] = strtof(k, NULL);
    }
  }
  // printf("\npassou\n");
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