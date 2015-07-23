
echo "------- Times --------" > Times.txt
for threads in 2 4 8 12 16
do 
	echo "------- $threads Threads --------" >>Times.txt
	mpirun -np $threads mpi >> Times.txt
	mpirun -np $threads mpi >> Times.txt
	mpirun -np $threads mpiP
done 

