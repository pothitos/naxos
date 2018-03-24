#!/bin/sh

: ${HD_HOME?"Please set the envinonment variable HD_HOME, by executing the command e.g. 'export HD_HOME=/usr/local/hd'"}

if [ $# -ne 6 -a $# -ne 7 ]
then
        echo "Usage: $0 CSP N MaxSplitTime SplitTime SimulationRatio Mappers" >&2
        exit 1
fi

CSP=$1
N=$2
MAXSPLITTIME=$3
SPLITTIME=$4
SIMRATIO=$5
MAPPERS=$6

ROUND=0

print_splits () {
        echo
        /bin/echo -e "\e[1m=== Round $ROUND: Fight! ===\e[0m"
}

if [ $# -eq 7 -a "$7" = "VALGRIND" ]
then
        VALGRIND="valgrind -q"
fi

echo "Split: 0 -" | \
        time -f "=== Initialization in %e seconds ===" \
          $VALGRIND ./$CSP $N $MAXSPLITTIME $SPLITTIME $SIMRATIO | \
          { tee /dev/fd/3 | grep "^Split: " > splits ;
          } 3>&1 | egrep "^Solve|^Simulate" > nodes.dat
          { awk -f progress.awk nodes.dat > progress.dat ;
            gnuplot progress.plt ;
          } &

while [ -s splits ]
do
        ROUND=$((ROUND+1))
        print_splits
        if [ $# -eq 7 ]
        then
                rm -f splits_*
                SPLIT_LINES=$(wc -l < splits)
                split --numeric-suffixes=1 -l $(((SPLIT_LINES+MAPPERS-1)/MAPPERS)) splits splits_partition_
                wc -l splits*
                rm splits
                for S in splits_*
                do
                        echo "./$CSP $N $MAXSPLITTIME $SPLITTIME $SIMRATIO < $S"
                        $VALGRIND ./$CSP $N $MAXSPLITTIME $SPLITTIME $SIMRATIO < $S | \
                        { tee /dev/fd/3 | grep "^Split: " >> splits ;
                        } 3>&1 | egrep "^Solve|^Simulate" >> nodes.dat
                        { awk -f progress.awk nodes.dat > progress.dat ;
                          gnuplot progress.plt ;
                        } &
                done
                continue
        fi
        time -f "=== Splits upload in %e seconds ===" \
                hadoop fs -moveFromLocal -f splits
        time -f "=== Hadoop round $ROUND in %e seconds ===" \
                hadoop jar $HD_HOME/share/hadoop/tools/lib/hadoop-streaming*.jar  \
                -D mapreduce.job.maps=$MAPPERS \
                -mapper "$PWD/$CSP $N $MAXSPLITTIME $SPLITTIME $SIMRATIO" \
                -reducer NONE \
                -input splits \
                -output solutions 2>&1 | \
            GREP_COLOR="1" egrep --color "Map input records=.*|$"
        time -f "=== Splits download in %e seconds ===" \
                hadoop fs -cat solutions/* | \
                { tee /dev/fd/3 | grep "^Split: " > splits ;
                } 3>&1 | egrep "^Solve|^Simulate" >> nodes.dat
                { awk -f progress.awk nodes.dat > progress.dat ;
                  gnuplot progress.plt ;
                } &
        hadoop fs -rm -r solutions
done
rm -f nodes.dat progress.dat splits*
