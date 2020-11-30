# Relatório do Laboratório 2
 ### Grupo: The Last Dance (The Man, The Myth, The Beast)
 
 ![alt text]( https://i.imgur.com/pDFm0Mr.png "The Man, The Myth, The Beast")


##### Eliseu Pereira Henrique de Paula - 215293

Para o laboratório 2 o grupo teve de resolver o dado problema utilizando a biblioteca **OpenACC**. 
O código feito para a resolução do dado problema, pode ser dividido em quatro etapas, sendo elas: leitura dos dados, realizar o cálculo proposto, realizar a redução por soma da matriz D e fazer a escrita no arquivo de texto da mesma. 

A compilação do código deve ser feita utilizando o makefile, simplesmente rodando o comando make dentro da pasta src irá gerar o executável na pasta bin. Para a execução do programa deve-se executar seguindo modelo mostrado abaixo:

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


Como dito anteriormente a execução do código pode ser dividida em cinco partes:
- Antes da primeira parte o código lê as variáveis que foram inicializadas na linha de comando do programa e as armazena em variáveis dentro do programa
1. A primeira parte do código envia as matrizes, que foram alocadas dinamicamente usando uma etapa somente, para a função `readMatrix`. Essa função irá entrar no dado arquivo para que seus dados sejam armazenados em uma matriz dentro do programa, fazendo isso para as matrizes A, B e C.
- A segunda, terceira e quarta parte do código é onde será utilizada a biblioteca **OpenACC**. 
2. A segunda parte do código envia as matrizes, que foram alocadas dinamicamente usando uma etapa somente, para a função `#pragma acc enter data copyin(matrizA, matrizB, aux)`. Essa função envia as matrizes A, B e aux para a memória da GPU. Logo após isso se inicia uma zona paralela do código com a função `#pragma acc parallel` e dentro dessa função se inicia um loop com `#pragma acc loop collapse(2)` onde esta realiza a multiplicação de matrizes A * B resultando na matriz aux. Logo após o cálculo, a regiao paralela acaba e é usada da função `#pragma acc exit data delete (matrizA, matrizB)` para apagar da memória da GPU as matrizes já utilizadas, após isso as mesmas matrizes são desalocadas no código com a função `free(matriz)`.
3. A terceira parte do código também começa com um função de entrada de dados na GPU, sendo ela `#pragma acc enter data copyin(aux, matrizC, matrizD)`, após essa função o codigo segue igual a segunda parte do código, entrando em uma zona paralela e seguindo para a função de loop, onde é realizada a multiplicação das matrizes aux * C resultando na matriz D. A terceira parte termina com `#pragma acc exit data delete (aux, matrizC)` apagando da memória da GPU as matrizes já utilizadas seguindom com a função `free` para desalocar as mesmas da memória.
4. Na quarta parte do código é realizada a redução por soma da matriz resultante da equação. Foi utilizada da "função" `#pragma acc parallel loop collapse(2) reduction(+ : soma)` onde ela separa o loop de soma em várias regiões paralelas da GPU, no final somando o valor de soma em uma única variável. Após a região paralela a matriz D é copiada de volta para a memória usando da função `#pragma acc exit data copyout(matrizD)`.
5. Na última etapa do código é realizada a escrita no arquivo utilizando da função `writeMatrix`. Essa função ira entrar no dado arquivo e escrever nele os dados da matriz que for enviada para a função, no caso a matriz resultante da equação.
- Após as seguintes etapas o programa irá escrever na tela o valor encontrado na soma dos valores da matriz resultante.


Para testar o código e calcular o tempo de execução foram utilizados os parâmetros propostos no problema, sendo eles: 
- y = 10, w = 10, v = 10
- y = 100, w = 100, v = 100
- y = 1000, w = 1000, v = 1000

Após o teste foi criado um gráfico mostrando os tempos de execução de cada dos parâmetros e comparando com o tempo do laboratório anterior:
![alt text]( https://docs.google.com/spreadsheets/u/1/d/e/2PACX-1vS-IUIbwi4TUUTUsI6E_X02x5UrpaSzoVF2DIWsBRMGeSStl2a7GJZ9VRyakJueZ4c2U-z9QwUJz2wr/pubchart?oid=539508022&format=image "Gráfico obtido")

Foi usada uma escala logarítmica no grafico para a melhor visualização dos dados.

- Na operação onde y, w e v eram 10 foi obtido o tempo médio de 0,014 milisegundos.
- Na operação onde y, w e v eram 100 foi obtido o tempo médio de 4,9 milisegundos.
- Na operação onde y, w e v eram 1000 foi obtido o tempo médio de 6181,439 milisegundos.

Acredito que essa grande diferença de performance entre as Bibliotecas OpenMP e OpenACC se dá ao fato das operações de copyin e copyout para levar os dados à GPU, tanto que quando são poucos os dados (y = 10, w = 10, v = 10), a execução é cerca de cem vezes mais rápida que a mesma em OpenMP, porém com o aumento dos dados ela se torna exponencialmente mais demorada. Acredito que se fosse medido somente o tempo das operações de multiplicação sem contar o tempo das das operações de cópia da GPU o tempo poderia ser bastante reduzido. Queria ressaltar que foi realizado testes utilizado de `acc kernels` e o tempo foi praticamente o dobro do uso `acc parallel`, e também que caso fosse usado `collapse(3)` nas operações de multiplicação o tempo de execução subia para aproximadamente 20 segundos. A solução utilizando de `acc enter data` e `acc exit data` foi a que obteve o melhor desempenho em questão de tempo.
