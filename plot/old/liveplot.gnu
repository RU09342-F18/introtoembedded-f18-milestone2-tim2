# Greg Ziegan (grz5)
# Matt Prosser (mep99)
# Plotting format file. Is reread to refresh live plot

reset
#set terminal wxt size 1300,600
### Plot x1, y1
set title "X1, Y1 Values"
unset key
stats 'plot.dat' using 3 name "X"
stats 'plot.dat' using 2 name "Y"
set xlabel "time (ms)"
set ylabel "delta (us)"
set y2label "RPM"

if (X_max < 10000000) {
    set autoscale x
} else {
    set xrange [X_max-10000000:X_max]
}

if (Y_max > 45000) {
    set autoscale y
} else {
    set yrange [0:45000]
}

set y2range [0:2000]

#set yrange [0:] 
plot "plot.dat" using 3:2 with lines axes x1y1, "plot.dat" using 3:5 with lines axes x1y2
###

pause 0.01  # stepsize
reread
