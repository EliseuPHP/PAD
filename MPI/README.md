# Relatório do Laboratório 2
 ### Grupo: The Last Dance (The Man, The Myth, The Beast)
 
 ![alt text]( https://i.imgur.com/pDFm0Mr.png "The Man, The Myth, The Beast")


##### Eliseu Pereira Henrique de Paula - 215293

Para o laboratório 3, o grupo teve de resolver o problema dado utilizando a biblioteca **MPI**. O código elaborado para a resolução do problema pode ser dividido em quatro etapas, sendo elas: leitura dos dados; realização das multiplicação de matrizes **(A * B = aux)** e **(aux * C = D)**; redução por soma da matriz **D**; fazer a escrita no arquivo de saída da mesma. 

A compilação do código deve ser feita utilizando o **makefile**, simplesmente rodando o comando **make** dentro da pasta **src** irá gerar o executável na pasta **bin**. Deve-se executar o programa seguindo o modelo apresentado abaixo:

`mpirun --hostfile ../hosts -np 8 ./programa y w v arqA.dat arqB.dat arqC.dat arqD.dat`

**Onde:**
- mpirun é o comando para executar o programa que usa a biblioteca MPI.
- --hostfile é um comando para indicar onde está o arquivo de configuração hosts.
- ../hosts é o local do arquivo de configuração hosts.
- -np é o comando para definir em quantos nós o programa deve rodar.
- 8 é a quantidade de nós disponíveis para a execução.
- ./programa é o programa que resolverá o problema.
- y é o número de linhas da primeira matriz.
- w é o número de colunas da primeira matriz e de linhas da segunda matriz.
- v é o número de colunas da segunda matriz e de linhas da terceira matriz.
- arqA.dat é o nome do arquivo que contém a primeira matriz.
- arqB.dat é o nome do arquivo que contém a segunda matriz.
- arqC.dat é o nome do arquivo que contém a terceira matriz.
- arqD.dat é o nome do arquivo que contém a matriz resultante da computação.

Como relatado anteriormente, a execução do código pode ser dividida em quatro partes:
- Antes da primeira parte, o código lê as variáveis que foram inicializadas na linha de comando do programa e as armazena em variáveis dentro do programa.
1. Iniciando a **Master Task**, a primeira parte do código envia as matrizes, as quais foram alocadas dinamicamente e usando somente uma etapa, para a função `readMatrix`. Essa função entrará no arquivo para que seus dados sejam armazenados em uma matriz dentro do programa, realizando essa operação para as matrizes A, B e C.
2. A segunda parte do código divide as matrizes baseada no número de **workers** (número de nós excluindo o nó **master**) para que as mesmas sejam enviadas para as **Worker Tasks** utilizando da função `MPI_Send(&matriz[Offset], Rows * Columns, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);`. Essa função se encontra dentro de um **for loop** que calcula o **offset** das matrizes do tipo **float** e as envia para as **Worker Tasks**. Imediatamente após o envio das matrizes, as **Worker Tasks** recebem as matrizes enviadas utilizando da função `MPI_Recv(matriz, Rows * Columns, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD, &status);`; após essa função se inicia um **loop** no qual será realizada a multiplicação de matrizes. Após o cálculo, a matriz resultante é enviada de volta para a **Master Task** utilizando da função `MPI_Send(matrizResultante, Rows * Columns, MPI_FLOAT, MASTER, mtype, MPI_COMM_WORLD);`. Logo em seguida a matriz é recebida na **Master Task** pela função `MPI_Recv(&matrizResultante[Offset], Rows * Columns, MPI_FLOAT, fonte, mtype, MPI_COMM_WORLD, &status);`. Essa função se encontra dentro de um **loop** onde a matriz será reconstruída baseada no **offset**. Todo esse processo é utilizado para os cálculos **A * B = aux** e **aux * C = D**.; em seguida as matrizes que não serão mais necessárias são desalocadas da memória com a função `free`.
3. Na terceira parte do código é realizada a redução por soma da matriz resultante da equação. É utilizado do mesmo processo descrito na segunda etapa para o envio da matriz pela **Master Task** e seu recebimento nas **Worker Tasks**. Após a matriz resultante ser recebida nas **Worker Tasks**, a matriz é enviada para um **loop** para que seja calculado um valor parcial da redução por soma (é calculado um valor parcial porque a matriz é dividida entre as **Worker Tasks**). Em seguida o valor parcial é enviado para a **Master Task** utilizando das funções `MPI_Send` (dentro das **Worker Tasks**) e `MPI_Recv` (dentro da **Master Task**); esse valor é recebido em um loop que realizará a soma dos valores parciais para que seja obtido o valor total da redução por soma.
4. Na última etapa do código é realizada a escrita no arquivo utilizando a função `writeMatrix`. Essa função entra no arquivo disponibilizado e escreve nele os dados da matriz que for enviada para a função - no caso, a matriz resultante da equação.
- Após as etapas previamente demonstradas, o programa escreverá na tela o valor da soma dos valores da matriz resultante.


Para testar o código e calcular o tempo de execução foram utilizados os parâmetros propostos no problema, sendo estes: 
- y = 10, w = 10, v = 10
- y = 100, w = 100, v = 100
- y = 1000, w = 1000, v = 1000

Após o teste foi criado um gráfico mostrando os tempos de execução de cada dos parâmetros e comparando com o tempo do laboratório anterior:

![alt text]( https://docs.google.com/spreadsheets/d/e/2PACX-1vS-IUIbwi4TUUTUsI6E_X02x5UrpaSzoVF2DIWsBRMGeSStl2a7GJZ9VRyakJueZ4c2U-z9QwUJz2wr/pubchart?oid=750307875&format=image "Gráfico obtido")

Foi usada uma escala logarítmica no gráfico para a melhor visualização dos dados.

- Na operação onde y, w e v eram 10 foi obtido o tempo médio de 41,586 milisegundos.
- Na operação onde y, w e v eram 100 foi obtido o tempo médio de 47,912 milisegundos.
- Na operação onde y, w e v eram 1000 foi obtido o tempo médio de 987,360 milisegundos.

Acredito que essa grande diferença nos tempos de execução com uma pequena quantidade de dados (tamanhos 10 e 100) se dá por causa dos nós estarem conectados em uma rede, então existe uma latência na transferência de dados. Falando sobre o tempo de processamento dos dados, excluindo a latência na transferência de dados, acredito que o tempo seja bem baixo, pois como podemos observar no gráfico, quando possuímos o tamanho de dados igual a 1000, o tempo se torna cerca de um segundo e meio menor que o mesmo processamento com **OpenMP**, ou seja, no caso em que temos o maior tempo de transferência de dados, a biblioteca **MPI** consegue o melhor tempo. Podemos concluir que para um grande número de dados a biblioteca **MPI** é a que tem a melhor performance.