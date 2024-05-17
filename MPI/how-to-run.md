### Come compilare
mpicc <nome_file_c>.c -o <nome_eseguibile>

### Come eseguire
mpirun -np <numero_processori> ./<nome_eseguibile>

### Compila ed esegue contemporaneamente
mpicc <nome_file_c>.c -o <nome_eseguibile> && mpirun -np 4 ./<nome_eseguibile>