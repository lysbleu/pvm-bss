for i in 1 2 3 4
do
    file=mpi_$(($i*$i)).txt
    echo "# Données pour $(($i*$i)) processus" > $file 
    for j in 2 5 8 10 12 13 14 15 17 19 20 21 22
    do
        echo "#Test pour $(($i*$i)) processus et une matrice $((36*$j))x$((36*$j))" >> $file
        duree=`/usr/bin/time -f "%e" mpiexec -np $(($i*$i)) $1 $((36*$j)) 2>&1 >/dev/null`
	echo "$((36*$j)) $duree" >> $file 
	echo "Sortie sur $file"
    done
done
