#! /bin/sh
set -ev
# Set parameters for small CSP instances
sed -i.bak 's/^\(NQUEENS_RANGE\)=.*/\1=$(seq 10 11)/' experiments.sh
sed -i 's/^\(MAGIC_SQUARE_RANGE\)=.*/\1=$(seq 5 6)/' experiments.sh
sed -i 's/^\(CREW_SCHEDULING_RANGE\)=.*/\1=OR-Library\/NW41.txt/' experiments.sh
sed -i 's/^\(TSP_RANGE\)=.*/\1=$(seq 8 9)/' experiments.sh
# Execute the measurements
./experiments.sh > ACvsBC.dat
cat ACvsBC.dat
# Restore the original script
mv experiments.sh.bak experiments.sh
# Graphically plot the results
./ACvsBC.plt
# Clean up
rm ACvsBC.tex ACvsBC.eps
