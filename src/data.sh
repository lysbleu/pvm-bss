#!bin/bash
for ((i=1;i<100;i+=2))
do
 for ((j=1; j<9; j++))
 do
   ./craquage $i $j p>> data/data_$i.txt;
 done
done