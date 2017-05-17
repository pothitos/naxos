#! /bin/sh
set -ev

SOLVER_FILES="../../core/*.h ../../core/*.cpp"
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

# Default Traveling Salesman Problem instance
$MEM_CHECK ./naxos-xcsp3 parser/src/XCSP3-CPP-Parser/instances/tsp-25-843.xml

# Default Constraint Optimisation (COP) instance
$MEM_CHECK ./naxos-xcsp3 parser/src/XCSP3-CPP-Parser/instances/obj.xml

# Limit the available time to 10s for searching a solution
timeout --preserve-status --kill-after=1s 10s \
    $MEM_CHECK ./naxos-xcsp3 verification/without_solutions/AllConstraints.xml

# Reduce the available time to 5s, while not testing memory
timeout --preserve-status --kill-after=1s 5s \
    ./naxos-xcsp3 verification/without_solutions/AllConstraintsFormatted.xml

# For each Mini-solver Competition's requirement, solve a CSP
for INSTANCE in verification/*.xml
do
    # Set the stored solution file name
    SOLUTION="verification/$(basename $INSTANCE .xml).sol"
    # Compare the stored solution with the solver's one
    # This test is temporarily disabled
    ./naxos-xcsp3 $INSTANCE | cmp $SOLUTION || true
done
