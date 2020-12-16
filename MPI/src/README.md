mpirun --hostfile ../hosts -np 8 ./programa.o 5 4 3 arqA.dat arqB.dat arqC.dat arqD.dat

scp programa.o workernode:/home/e196396/PAD/MPI/bin/programa.o