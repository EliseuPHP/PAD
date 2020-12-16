mpirun --hostfile ../hosts -np 8 ./programa.o 100 100 100 arqA.dat arqB.dat arqC.dat arqD.dat

scp programa.o workernode:/home/e196396/PAD/MPI/bin/programa.o