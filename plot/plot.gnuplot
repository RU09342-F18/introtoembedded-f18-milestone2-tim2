reset

unset key

stats 'data' using 1 name "X" nooutput

set xrange [X_max-50000:X_max]
set yrange [0 : 100]
set y2range [0 : 10000]

plot 'data' using 1:3 with lines, 'data' using 1:4 with lines axes x1y2

pause 0.01
reread

