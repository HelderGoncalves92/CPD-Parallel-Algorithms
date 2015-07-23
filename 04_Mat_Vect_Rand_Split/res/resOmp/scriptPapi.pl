
export OMPP_CTR1=PAPI_L1_TCM
export OMPP_CTR2=PAPI_L2_TCM
export OMPP_CTR3=PAPI_L3_TCM
export OMPP_CTR4=PAPI_TOT_INS

export OMPP_OUTFORMAT=CSV

x=8000000 y=8
echo "------- Dim: $x $y-------" > Times.txt
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	echo "-->Nº threads: $nthreads" >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./ompP $nthreads $x $y
	
done



x=8000 y=8000
echo "------- Dim: $x $y-------" >> Times.txt
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	echo "-->Nº threads: $nthreads" >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./ompP $nthreads $x $y
done

x=8 y=8000000
echo "------- Dim: $x $y-------" >> Times.txt
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	echo "-->Nº threads: $nthreads" >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./omp $nthreads $x $y >> Times.txt
	./ompP $nthreads $x $y
done

