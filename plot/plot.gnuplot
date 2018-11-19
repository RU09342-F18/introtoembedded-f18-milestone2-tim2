reset

unset key

stats 'data' using 1 name "X" nooutput

set xrange [X_max-600:X_max]
set yrange [0 : 100]

plot 'data' using 1:2 with lines

pause 0.01
reread

