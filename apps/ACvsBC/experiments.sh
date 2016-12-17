#! /bin/sh
set -e

echo "CSP\tParam\tn\td\te\tAC_Time\tBC_Time"
echo
for PARAM in $(seq 10 15)
do
    time -o AC_Time.txt -f "%e" ./nqueens.AC $PARAM > /dev/null
    time -o BC_Time.txt -f "%e" ./nqueens $PARAM | tail -1 > n_d_e.txt
    echo "NQueens\t$PARAM\t$(cat n_d_e.txt)\t$(cat AC_Time.txt)\t$(cat BC_Time.txt)"
done
echo
for PARAM in $(seq 7 9)
do
    time -o AC_Time.txt -f "%e" ./magic_square.AC $PARAM > /dev/null
    time -o BC_Time.txt -f "%e" ./magic_square $PARAM | tail -1 > n_d_e.txt
    echo "MagicSq\t$PARAM\t$(cat n_d_e.txt)\t$(cat AC_Time.txt)\t$(cat BC_Time.txt)"
done
echo
for PARAM in OR-Library/*
do
    time -o AC_Time.txt -f "%e" ./crew_scheduling.AC $PARAM > /dev/null
    time -o BC_Time.txt -f "%e" ./crew_scheduling $PARAM | tail -1 > n_d_e.txt
    echo "CrewSch\t$(basename $PARAM .txt)\t$(cat n_d_e.txt)\t$(cat AC_Time.txt)\t$(cat BC_Time.txt)"
done
echo
for PARAM in $(seq 9 14)
do
    time -o AC_Time.txt -f "%e" ./tsp.AC HA30.pl $PARAM > /dev/null
    time -o BC_Time.txt -f "%e" ./tsp HA30.pl $PARAM | tail -1 > n_d_e.txt
    echo "TSP\t$PARAM\t$(cat n_d_e.txt)\t$(cat AC_Time.txt)\t$(cat BC_Time.txt)"
done
rm n_d_e.txt AC_Time.txt BC_Time.txt
