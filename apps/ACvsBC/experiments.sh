#! /bin/sh

echo "#CSP\tParam\tn\td\te\tAC_Time\tBC_Time"
echo
PARAM=1
while [ $PARAM -le 100 ]
do
    time -o AC_Time.txt -f "%e" NQUEENS.AC $PARAM > /dev/null
    time -o BC_Time.txt -f "%e" NQUEENS $PARAM | tail -1 > n_d_e.
    echo "NQUEENS\t$PARAM\t$((cat n_d_e.txt))\t$((cat AC_Time.txt))\t$((cat BC_Time.txt))"
    PARAM=$(($PARAM + 1))
done
echo
for INSTANCE in NW41 NW32 NW40 NW08 NW15 NW21 NW22 NW12 NW39 NW20 NW23 NW37 NW26 NW10 NW34 NW43 NW42 NW28 NW25 NW38
do
    echo -n  "$INSTANCE\t"
    CSPs/CREW_SCHEDULING.AC "CSPs/OR-Library/$INSTANCE.txt"
    CSPs/CREW_SCHEDULING "CSPs/OR-Library/$INSTANCE.txt"
done
echo
INSTANCE=13
while [ $INSTANCE -le 14 ]
do
    echo
    w
    PRECISION=55
    while [ $PRECISION -le 100 ]
    do
        echo -n "TSP$INSTANCE\t$PRECISION\t"
        CSPs/TSP.AC CSPs/HA30.pl "$INSTANCE" "$PRECISION"
        CSPs/TSP CSPs/HA30.pl "$INSTANCE" "$PRECISION"
        if [ $PRECISION -eq 1 ]
        then
            PRECISION=0
        fi
        PRECISION=$(($PRECISION + 5))
    done
    INSTANCE=$(($INSTANCE + 1))
done
