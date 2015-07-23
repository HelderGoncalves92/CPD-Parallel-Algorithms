
cat $PBS_NODEFILE

for size in 100 1000 10000 50000 100000 200000 500000
do 
	echo "++++++++++++++++ Dimensão $size ++++++++++++++++++"
	for nthreads in 1 2 4 8 16 24
	do
		echo "Num threads: $nthreads "
		export OMP_NUM_THREADS=$nthreads
		./omp $size g
		./omp $size g
		./omp $size g
	done	
done
 
