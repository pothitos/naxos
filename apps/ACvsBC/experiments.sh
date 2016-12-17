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
rm n_d_e.txt AC_Time.txt BC_Time.txt
