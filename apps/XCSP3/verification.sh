#! /bin/sh
set -ev
SOLVER_FILES="../../core/bounds-consistency/*.h ../../core/bounds-consistency/*.cpp"
# Ensure that Naxos Solver can be considered as a "Mini-Solver".
# According to the First International XCSP3 Competition: "A
# mini-solver is a solver whose code must be composed of
# less than 8,000 lines of at most 160 characters (while
# discarding code for parsing XCSP3, comments and code of
# standard libraries)."
SLOC=$(cat $SOLVER_FILES | grep -v "^$" | grep -v "^ *//" | wc -l)
echo "$SLOC pure source lines of code"
test $SLOC -lt 8000
# Temporarily disable the maximum line width test
! grep ".\{161\}" $SOLVER_FILES || true
# Check for memory leaks
valgrind --leak-check=full --error-exitcode=1 ./naxos-xcsp3 > /dev/null
