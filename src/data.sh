#!bin/bash
for ((i=1;i<20;i++))
do
 for ((j=1; j<9; j++))
 do
   ./craquage $i $j p>> data/data_$i.txt;
 done
done