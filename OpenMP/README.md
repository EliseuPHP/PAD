# Relatório do Laboratório 1
 ### Grupo: The Last Dance (The Man, The Myth, The Beast)
 
 ![alt text]( https://i.imgur.com/pDFm0Mr.png "The Man, The Myth, The Beast")


##### Eliseu Pereira Henrique de Paula - 215293

Para o laboratório 1 o grupo teve de resolver o dado problema utilizando a boblioteca **OpenMP**. 
O código feito para a resolução do dado problema, pode ser dividido em quatro etapas, sendo elas: leitura dos dados, realizar o cálculo proposto, realizar a redução por soma da matriz D e fazer a escrita no arquivo de texto da mesma. 

A compilação do codigo deve ser feita utilizando o makefile, simplemente rodando o comando make dentro da pasta src irá gerar o executavel na pasta bin. Para a execução do programa deve-se executar seguindo modelo mostrado abaixo:

`./programa y w v arqA.dat arqB.dat arqC.dat arqD.dat`

**Onde:**
- ./programa é o programa que resolverá o problema.
- y é o número de linhas da primeira matriz.
- w é o número de colunas da primeira matriz e de linhas da segunda matriz.
- v é o número de colunas da segunda matriz e de linhas da terceira matriz.
- arqA.dat é o nome do arquivo que contém a primeira matriz.
- arqB.dat é o nome do arquivo que contém a segunda matriz.
- arqC.dat é o nome do arquivo que contém a terceira matriz. 
- arqD.dat é o nome do arquivo que contém a matriz resultante da computação. 


Como dito anteriormente a execução do código pode ser dividida em quatro partes:
- Antes da primeira parte o código lê as variáveis que foram inicializadas na linha de comando do programa e as armazena em variaveis dentro do programa
1. A primeira parte do código envia as matrizes, que foram alocadas dinamicamente usando uma etapa somente, para a função *readMatrix*. Essa função ira entrar no dado arquivo para que seus dados sejam armazenados em uma matriz dentro do programa, fazendo isso para as matrizes A, B e C.
2. A segunda parte do codigo é onde será utilizada a biblioteca **OpenMP**. Já alocada as três matrizes que serão usadas para a equação, o codigo entra na seguinte "função" `#pragma omp parallel shared(matrizA, matrizB, matrizC, aux, y, v) private(i, j, k)` onde essa irá paralelizar as threads do processador para que seja executada a equação. Após isso, em cada um dos `for` necessários para percorrer as matrizes existe a seguinte "função" `#pragma omp for` onde essa irá distribuir o loop entre as threads.
3. Na terceira parte do código é realizada a redução por soma da matriz resultante da equação. Foi utilizada da "função" `#pragma omp parallel for reduction(+ : soma)` onde ela separa o loop de soma para várias threads.
4. Na última etapa do código é realizada a escrita no arquivo utlizando da função *writeMatrix*. Essa função ira entrar no dado arquivo e escrever nele os dados da matriz que for enviada para a função, no caso a matriz resultante da equação.
- Após as seguintes etapas o programa irá escrever na tela o valor encontrado na soma dos valores da matriz resultante.


Para testar o código e calcular o tempo de execução foram utilizados os parâmetros propostos no problema, sendo eles: 
- y = 10, w = 10, v = 10
- y = 100, w = 100, v = 100
- y = 1000, w = 1000, v = 1000

Após o teste foi criado um gráfico mostrando os tempos de execução de cada dos parâmetros:
![alt text]( https://docs.google.com/spreadsheets/d/e/2PACX-1vS-IUIbwi4TUUTUsI6E_X02x5UrpaSzoVF2DIWsBRMGeSStl2a7GJZ9VRyakJueZ4c2U-z9QwUJz2wr/pubchart?oid=1684031500&format=image "Gráfico obtido")

- Na operação onde y, w e v eram 10 foi obtido o tempo médio de 0,126 milisegundos.
- Na operação onde y, w e v eram 100 foi obtido o tempo médio de 1,364 milisegundos.
- Na operação onde y, w e v eram 1000 foi obtido o tempo médio de 3539,051 milisegundos.

Pode ser observado que o aumento no tempo de execução não é linear e sim exponencial, porém, provavelmente pelo fato do programa ser realizado em várias threads torna a execução dele muito mais rápida, pensando que na primeira operação usamos matrizes com até 100 elementos em cada e na segunda operação com 10000 o tempo de execucao da segunda operação é cerca de dez vezes menos que o esperado.
