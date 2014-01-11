#!/usr/bin/bash
command -v sponge >/dev/null 2>&1 || { echo >&2 "Il faut installer moreutils" ; exit 1; }
command -v gnuplot >/dev/null 2>&1 || { echo >&2 "Il faut installer gnuplot"; exit 1; }
echo "# Fichier temporaire pour gnuplot" > plot_ddot
echo "set logscale x" >> plot_ddot
#~ echo "set size ratio -1" >> plot_ddot
#~ echo "set size square" >> plot_ddot
echo "set terminal pngcairo enhanced size 1000,650" >> plot_ddot
echo "set output 'plot_ddot.png'" >> plot_ddot
#~ echo "set key on center rmargin box title \"Nombre de\n processus\"" >> plot_ddot
echo "set xlabel \"Taille des vecteurs\"" >> plot_ddot
echo "set ylabel \"Vitesse d'exécution (MFlop/s)'\"" >> plot_ddot

###########################################################################
prefix=""
values=("dgemm_perf")
suffix=".txt"
###########################################################################

echo -n "plot " >> plot_ddot
for i in ${values[@]}
do
    echo "\"${prefix}$i${suffix}\" using 1:2 title \"0\" with lines,\\" >> plot_ddot
    echo "\"${prefix}$i${suffix}\" using 1:3 title \"1\" with lines,\\" >> plot_ddot
    echo "\"${prefix}$i${suffix}\" using 1:4 title \"2\" with lines,\\" >> plot_ddot
    echo "\"${prefix}$i${suffix}\" using 1:5 title \"3\" with lines,\\" >> plot_ddot
done

head -c-3 plot_ddot | sponge plot_ddot
gnuplot plot_ddot
echo "Sortie sur plot_ddot.png"
rm plot_ddot
