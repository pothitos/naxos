#! /bin/sh
set -ev
# Set default values, if variables are unset
DATASET=${DATASET-"SCEN06"}
# The second field of the output should be the objective value
./celar_solver $DATASET | grep "^[^	]*	[[:digit:]]\+	"
# Clean up
rm $DATASET/sol.txt
