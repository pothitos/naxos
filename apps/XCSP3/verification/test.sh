#! /bin/sh

validate () {
    # Solution validation tool
    VALIDATOR="java -classpath checker/src/XCSP3-Java-Tools/src/main/java
               org.xcsp.checker.SolutionChecker -cm"

    COST=$(grep "^o -\?[[:digit:]]\+$" $SOLUTION | tail -1 |
           grep -o -- "-\?[[:digit:]]\+" || true)
    VALIDATION=$($VALIDATOR $INSTANCE $SOLUTION)
    if [ "$VALIDATION" = "ERROR: the instantiation cannot be checked" ]
    then
        echo "$VALIDATION for $INSTANCE"
    elif [ "$VALIDATION" != "OK	$COST" ]
    then
        echo
        echo "Wrong solution for $INSTANCE:"
        cat $SOLUTION
        echo "$VALIDATION"
        #exit 1
        # Continue even the test fails
    fi
}

set -ev

SOLVER_FILES=$(ls ../../core/*.h ../../core/*.cpp | \
               grep -v "non_mini_solver\|naxos.h")
# Ensure that Naxos Solver can be considered as a "mini-solver".
# According to the First International XCSP3 Competition: "A
# mini-solver is a solver whose code must be composed of
# less than 8,000 lines of at most 160 characters (while
# discarding code for parsing XCSP3, comments and code of
# standard libraries)."
SLOC=$(cat $SOLVER_FILES | grep -v "^$" | grep -v "^ *//" | wc -l)
echo "$SLOC pure source lines of code"
test $SLOC -lt 8000

# Ensure that the maximum line width limit isn't exceeded
(! grep ".\{161\}" $SOLVER_FILES)

# Check coding style in TraviS CI
if [ "$CONTINUOUS_INTEGRATION" = "true" ]
then
    cd -
    # Fix coding style of all source files
    find \( -iname "*.h" -or -iname "*.cpp" \) \
        -exec clang-format-5.0 -i {} +
    # List the files that may need reformatting
    git ls-files -m
    cd -
fi

# Memory check tool
MEM_CHECK="valgrind -q"

# Temporary file
SOLUTION="/tmp/instance.sol"

# Default Traveling Salesman Problem instance
INSTANCE="parser/src/XCSP3-CPP-Parser/instances/tsp-25-843.xml"
$MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# Default Constraint Optimisation (COP) instance
INSTANCE="parser/src/XCSP3-CPP-Parser/instances/obj.xml"
$MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# Limit the available time to 10s for searching a solution
INSTANCE="verification/without_solutions/AllConstraints.xml"
timeout --preserve-status --kill-after=1s 10s \
    $MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# Reduce the available time to 5s, while not testing memory
INSTANCE="verification/without_solutions/AllConstraintsFormatted.xml"
timeout --preserve-status --kill-after=1s 5s \
    ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# Clean up
rm $SOLUTION

# For each Mini-solver Competition's requirement, solve a CSP
for INSTANCE in verification/*.xml
do
    # Set the stored solution file name
    SOLUTION="verification/$(basename $INSTANCE .xml).sol"
    # Compare the stored solution with the solver's one
    ./naxos-xcsp3 $INSTANCE | cmp $SOLUTION
    validate
done
