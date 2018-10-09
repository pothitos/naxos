#!/bin/sh -ev
# Set default values, if variables are unset
DATASET=${DATASET-"SCEN06"}
# If Valgrind is installed, do memory check too
if command -v valgrind
then
    MEM_CHECK="valgrind -q --leak-check=full --error-exitcode=1"
fi
# The second field of the output should be the objective value
$MEM_CHECK ./celar_solver $DATASET > solution.txt
grep "^[^	]*	[[:digit:]]\+	" solution.txt
# Clean up
rm $DATASET/sol.txt solution.txt
