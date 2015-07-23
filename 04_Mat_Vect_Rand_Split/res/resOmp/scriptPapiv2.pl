
export OMPP_CTR1=PAPI_LD_INS
export OMPP_CTR4=PAPI_SR_INS

export OMPP_OUTFORMAT=CSV

x=8000000 y=8
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	./ompP $nthreads $x $y
	
done



x=8000 y=8000
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	./ompP $nthreads $x $y
done

x=8 y=8000000
for nthreads in 1 2 4 8
do	
	export OMP_NUM_THREADS=$nthreads
	./ompP $nthreads $x $y
done

