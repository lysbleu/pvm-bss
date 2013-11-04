for i in 1 2 3 4
do
    for j in 10 30 50 100 300
    do
        echo "Test pour $(($i*$i)) processus et une matrice $(($i*$i*$j))x$(($i*$i*$j))"
        time -f "%e" mpiexec -np $(($i*$i)) $1 $(($i*$i*$j)) 2>&1 >/dev/null
    done
done
