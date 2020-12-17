mpirun --hostfile ../hosts -np 8 ./programa.o 5 4 3 arqA.dat arqB.dat arqC.dat arqD.dat

teste
mpirun --hostfile ../hosts -np 8 ./programa.o 997 981 991 arqA.dat arqB.dat arqC.dat arqD.dat

`./compara.sh resposta.dat arqD.dat`
`-4129376.518921`

scp programa.o workernode:/home/e196396/PAD/MPI/bin/programa.o