command -v sponge >/dev/null 2>&1 || { echo >&2 "Il faut installer moreutils" ; exit 1; }
command -v gnuplot >/dev/null 2>&1 || { echo >&2 "Il faut installer gnuplot"; exit 1; }

echo "set logscale y" > plot
echo "set terminal pngcairo size 800,500" >> plot
echo "set output 'plot.png'" >> plot
echo "set key on center rmargin box title \"Nombre de\n processus\"" >> plot
echo "set xlabel \"Longueur du mot (lettres)\"" >> plot
echo "set ylabel \"Temps (ms)\"" >> plot

###########################################################################
prefix=""
values=(1 3 5 7 11 17 21 31)
suffix=".txt"
###########################################################################

echo -n "plot " >> plot
for i in ${values[@]}
do
    echo "\"$prefix$i$suffix\" using 1:2 title \"$i\" with linespoints,\\" >> plot
done

head -c-3 plot | sponge plot
gnuplot plot
echo "Sortie sur plot.png"
