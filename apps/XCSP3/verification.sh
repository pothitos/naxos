#! /bin/sh
set -ev

# Check for memory leaks
MEM_CHECK="valgrind -q --leak-check=full --error-exitcode=1"
$MEM_CHECK ./naxos-xcsp3 > /dev/null

SOLVER_FILES="../../core/*.h ../../core/*.cpp"
# Ensure that Naxos Solver can be considered as a "Mini-Solver".
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

# Fix coding style of all source files
find ../.. \( -iname '*.h' -or -iname '*.cpp' \) -exec clang-format-5.0 -i {} +
# List the file that may need reformatting
cd -
git ls-files -m
cd -
