#!/usr/bin/bash
command -v sponge >/dev/null 2>&1 || { echo >&2 "Il faut installer moreutils" ; exit 1; }
command -v gnuplot >/dev/null 2>&1 || { echo >&2 "Il faut installer gnuplot"; exit 1; }
echo "# Fichier temporaire pour gnuplot" > plot_dgemm
#~ echo "set logscale y" >> plot_dgemm
#~ echo "set size ratio -1" >> plot_dgemm
#~ echo "set size square" >> plot_dgemm
echo "set terminal pngcairo enhanced size 1000,650" >> plot_dgemm
echo "set output 'plot_dgemm.png'" >> plot_dgemm
#~ echo "set key on center rmargin box title \"Nombre de\n processus\"" >> plot_dgemm
echo "set xlabel \"Temps d'éxécution du produit matriciel (us)\"" >> plot_dgemm
echo "set ylabel \"Taille des matrices (m * n)\"" >> plot_dgemm

###########################################################################
prefix="perf_dgemm"
values=("")
suffix=".txt"
###########################################################################

echo -n "plot " >> plot_dgemm
for i in *.txt
do
    echo "\"$i\" using 1:2 title \"$i\" with lines,\\" >> plot_dgemm
done

head -c-3 plot_dgemm | sponge plot_dgemm
gnuplot plot_dgemm
echo "Sortie sur plot_dgemm.png"
rm plot_dgemm
