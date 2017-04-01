#! /bin/sh
set -ev

# Check for memory leaks
MEM_CHECK="valgrind -q"
$MEM_CHECK ./naxos-xcsp3 parser/src/XCSP3-CPP-Parser/instances/tsp-25-843.xml \
    > /dev/null
$MEM_CHECK ./naxos-xcsp3 parser/src/XCSP3-CPP-Parser/instances/obj.xml > \
    /dev/null

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

# Check coding style in TraviS CI
if [ "$CONTINUOUS_INTEGRATION" = "true" ]
then
    # Fix coding style of all source files
    find ../.. \( -iname "*.h" -or -iname "*.cpp" \) \
        -exec clang-format-5.0 -i {} +
    # Use an individual coding style for Catch unit test macros:
    # Don't wrap braces after functions/macros
    sed -i.bak 's/AfterFunction: .*/AfterFunction: false/' ../../.clang-format
    clang-format-5.0 -i verification/naxos-xcsp3-test.cpp
    mv ../../.clang-format.bak ../../.clang-format
    # List the files that may need reformatting
    cd -
    git ls-files -m
    cd -
fi
