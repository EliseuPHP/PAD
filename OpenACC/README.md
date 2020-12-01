# Relatório do Laboratório 2
 ### Grupo: The Last Dance (The Man, The Myth, The Beast)
 
 ![alt text]( https://i.imgur.com/pDFm0Mr.png "The Man, The Myth, The Beast")


##### Eliseu Pereira Henrique de Paula - 215293

Para o laboratório 2, o grupo teve de resolver o problema dado utilizando a biblioteca **OpenACC**. O código elaborado para a resolução do problema pode ser dividido em cinco etapas, sendo elas: leitura dos dados; realização do cálculo proposto **(A * B = aux)**; realização do cálculo proposto **(aux * C = D)**; redução por soma da matriz **D**; fazer a escrita no arquivo de de saída da mesma. 

A compilação do código deve ser feita utilizando o **makefile**, simplesmente rodando o comando **make** dentro da pasta **src** irá gerar o executável na pasta **bin**. Deve-se executar o programa seguindo o modelo apresentado abaixo:

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


Como relatado anteriormente, a execução do código pode ser dividida em cinco partes:
- Antes da primeira parte, o código lê as variáveis que foram inicializadas na linha de comando do programa e as armazena em variáveis dentro do programa.
1. A primeira parte do código envia as matrizes, as quais foram alocadas dinamicamente e usando somente uma etapa, para a função `readMatrix`. Essa função irá entrará no arquivo para que seus dados sejam armazenados em uma matriz dentro do programa, ealizando essa operação para as matrizes A, B e C.
- As segunda, terceira e quarta partes do código são aquelas onde a biblioteca **OpenACC** é utilizada. 
2. A segunda parte do código envia as matrizes para a função `#pragma acc enter data copyin(matrizA, matrizB, aux)`. Essa função envia as matrizes A, B e aux para a memória da GPU. Imediatamente após o envio das matrizes, se inicia uma zona paralela do código com a função `#pragma acc parallel`; dentro dessa função se inicia um **loop** com `#pragma acc loop collapse(2)` no qual esta realiza a multiplicação de matrizes **A * B** resultando na matriz **aux**. Logo após o cálculo, a região paralela é finalizada e é empregada a função `#pragma acc exit data delete (matrizA, matrizB)` para apagar da memória da GPU as matrizes já utilizadas.; a seguir as mesmas matrizes são desalocadas da memória com a função `free`.
3. A terceira parte do código também começa com uma função de entrada de dados na GPU, `#pragma acc enter data copyin(aux, matrizC, matrizD)`; Após a execução dessa função o código prosegue de maneira idêntica à segunda parte, entrando em uma zona paralela e seguindo para a função de **loop**, onde é realizada a multiplicação das matrizes **aux * C** resultando na matriz **D**. A terceira parte termina com `#pragma acc exit data delete (aux, matrizC)` apagando da memória da GPU as matrizes já utilizadas seguindom com a função `free` para desalocar as mesmas da memória.
4. Na quarta parte do código é realizada a redução por soma da matriz resultante da equação. Foi utilizada a "função" `#pragma acc parallel loop collapse(2) reduction(+ : soma)`, que separa o **loop** de soma em várias regiões paralelas da GPU, ao final somando o valor totalizado em uma única variável. Após a região paralela a matriz D é copiada de volta para a memória usando a função `#pragma acc exit data copyout(matrizD)`.
5. Na última etapa do código é realizada a escrita no arquivo utilizando a função `writeMatrix`. Essa função entra no arquivo disponibilizado e escreve nele os dados da matriz que for enviada para a função - no caso, a matriz resultante da equação.
- Após as etapas previamente demonstradas, o programa irá escreverá na tela o valor da soma dos valores da matriz resultante.


Para testar o código e calcular o tempo de execução foram utilizados os parâmetros propostos no problema, sendo estes: 
- y = 10, w = 10, v = 10
- y = 100, w = 100, v = 100
- y = 1000, w = 1000, v = 1000

Após o teste foi criado um gráfico mostrando os tempos de execução de cada dos parâmetros e comparando com o tempo do laboratório anterior:
![alt text]( https://docs.google.com/spreadsheets/u/1/d/e/2PACX-1vS-IUIbwi4TUUTUsI6E_X02x5UrpaSzoVF2DIWsBRMGeSStl2a7GJZ9VRyakJueZ4c2U-z9QwUJz2wr/pubchart?oid=539508022&format=image "Gráfico obtido")

Foi usada uma escala logarítmica no gráfico para a melhor visualização dos dados.

- Na operação onde y, w e v eram 10 foi obtido o tempo médio de 0,014 milisegundos.
- Na operação onde y, w e v eram 100 foi obtido o tempo médio de 4,9 milisegundos.
- Na operação onde y, w e v eram 1000 foi obtido o tempo médio de 6181,439 milisegundos.

Acredito que essa grande diferença de performance entre as Bibliotecas **OpenMP** e **OpenACC** se dá em decorrência das operações de **copyin** e **copyout** para levar os dados à GPU. Quando a quantidade de dados é relativamente pequena (y = 10, w = 10, v = 10), a execução do programa é cerca de cem vezes mais rápida que execução do mesmo programa em OpenMP. Porém, com o aumento do número de dados ela se torna exponencialmente mais demorada. Acredito que se fosse medido somente o tempo das operações de multiplicação sem contar o tempo das das operações de cópia da GPU o tempo poderia ser bastante reduzido. É importante também ressaltar que foram realizados testes utilizado `acc kernels` e o tempo de execução foi praticamente o dobro de quando se utilizou `acc parallel`. Quando o `collapse(3)` foi usado nas operações de multiplicação, o tempo de execução subiu para aproximadamente 20 segundos. A solução utilizando `acc enter data` e `acc exit data` foi a que obteve o melhor desempenho com relação ao parâmetro de tempo de execução.
