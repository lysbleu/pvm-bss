#!/usr/bin/bash
command -v sponge >/dev/null 2>&1 || { echo >&2 "Il faut installer moreutils" ; exit 1; }
command -v gnuplot >/dev/null 2>&1 || { echo >&2 "Il faut installer gnuplot"; exit 1; }
echo "# Fichier temporaire pour gnuplot" > plot
#~ echo "set logscale y" >> plot
echo "set size ratio -1" >> plot
#~ echo "set size square" >> plot
echo "set terminal pngcairo enhanced size 1000,650" >> plot
echo "set output 'plot.png'" >> plot
#~ echo "set key on center rmargin box title \"Nombre de\n processus\"" >> plot
echo "set xlabel \"Temps d'éxéction du produit scalaire (us)\"" >> plot
echo "set ylabel \"Taille des vecteurs\"" >> plot

###########################################################################
prefix="perf_"
values=("ddot")
suffix=".txt"
###########################################################################

echo -n "plot " >> plot
for i in *.txt
do
    echo "\"$i\" using 1:2 title \"$i\" with lines,\\" >> plot
done

head -c-3 plot | sponge plot
gnuplot plot
echo "Sortie sur plot.png"
