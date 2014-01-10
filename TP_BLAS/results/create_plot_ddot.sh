#!/usr/bin/bash
command -v sponge >/dev/null 2>&1 || { echo >&2 "Il faut installer moreutils" ; exit 1; }
command -v gnuplot >/dev/null 2>&1 || { echo >&2 "Il faut installer gnuplot"; exit 1; }
echo "# Fichier temporaire pour gnuplot" > plot_ddot
#~ echo "set logscale y" >> plot_ddot
#~ echo "set size ratio -1" >> plot_ddot
#~ echo "set size square" >> plot_ddot
echo "set terminal pngcairo enhanced size 1000,650" >> plot_ddot
echo "set output 'plot_ddot.png'" >> plot_ddot
#~ echo "set key on center rmargin box title \"Nombre de\n processus\"" >> plot_ddot
echo "set xlabel \"Temps d'éxécution du produit scalaire (us)\"" >> plot_ddot
echo "set ylabel \"Taille des vecteurs\"" >> plot_ddot

###########################################################################
prefix="perf_ddot"
values=("")
suffix=".txt"
###########################################################################

echo -n "plot " >> plot_ddot
for i in *.txt
do
    echo "\"$i\" using 1:2 title \"$i\" with lines,\\" >> plot_ddot
done

head -c-3 plot_ddot | sponge plot_ddot
gnuplot plot_ddot
echo "Sortie sur plot_ddot.png"
rm plot_ddot
