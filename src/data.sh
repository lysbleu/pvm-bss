#!bin/bash
for ((i=100;i>3;i-=3))
do
 for ((j=1; j<11; j++))
 do
   ./craquage $i $j p>> data/data_$i.txt;
 done
done