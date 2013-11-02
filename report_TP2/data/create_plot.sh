echo "Il faut installer moreutils"
echo "set logscale y" > plot
#echo "unset key" >> plot
values=(1 3 5 7 11 17 21 31)
echo -n "plot " >> plot
for i in ${values[@]}
do
    echo "\"$i.txt\" using 1:2 title \"$i\" with linespoints,\\" >> plot
done

head -c-3 plot | sponge plot

