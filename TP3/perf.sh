#!/bin/bash
rm perf/perf_*.txt
for i in 48 40 30 20 10 1
do
	for j in 80 75 70 75 60 55 50 45 40 35 30 25 20 15 10 5 1
	do
		genere_data j
		duree=`/usr/bin/time -f "%e" mpiexec -np $i ./interaction ./data/data_$j.txt 1000`
		echo "$j*$j $duree" >> ./perf/perf_$i.txt 
		#~ echo "Sortie sur $file"
    done
done
