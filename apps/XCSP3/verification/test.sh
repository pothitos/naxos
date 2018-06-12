#!/bin/sh

install_validator() {
    # Install XCSP3 Checker
    cd verification/
    if [ ! -e XCSP3-Java-Tools ]
    then
        git clone https://github.com/xcsp3team/XCSP3-Java-Tools.git
    fi
    cd XCSP3-Java-Tools/
    git pull --ff-only
    gradle build -x test
    cd ../../
}

. verification/common_functions.sh

set -ev

# Check coding style in Travis CI
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

install_validator

# For each Mini-solver Competition's requirement, solve a CSP
for INSTANCE in verification/*.xml
do
    # Set the stored solution file name
    SOLUTION="verification/$(basename $INSTANCE .xml).sol"
    # Compare the stored solution with the solver's one
    ./naxos-xcsp3 $INSTANCE | cmp $SOLUTION
    validate
done

# Memory check tool
MEM_CHECK="valgrind -q --error-exitcode=1"

# Temporary file
SOLUTION="/tmp/instance.sol"

# XCSP3 Parser's Traveling Salesman Problem instance
INSTANCE="parser/src/XCSP3-CPP-Parser/instances/tsp-25-843.xml"
set +e  # Temporarily allow errors
timeout --preserve-status --kill-after=1s 15s \
    $MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate_exit_status $?

# XCSP3 Parser's Constrained Optimization (COP) instance
INSTANCE="parser/src/XCSP3-CPP-Parser/instances/obj.xml"
$MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# Limit the available time to 10s for searching a solution
INSTANCE="verification/without_solutions/AllConstraints.xml"
set +e  # Temporarily allow errors
timeout --preserve-status --kill-after=1s 10s \
    $MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate_exit_status $?

# Reduce the available time to 5s, while not testing memory
INSTANCE="verification/without_solutions/AllConstraintsFormatted.xml"
set +e  # Temporarily allow errors
timeout --preserve-status --kill-after=1s 5s \
    ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate_exit_status $?

# XCSP3 Checker's instances
for INSTANCE in $(cat verification/CheckerFastInstances.txt)
do
    unlzma --keep $INSTANCE.lzma
    ./naxos-xcsp3 $INSTANCE > $SOLUTION
    validate
    rm $INSTANCE
done

# XCSP3 Checker's instances that take more time
for INSTANCE in $(cat verification/CheckerSlowInstances.txt)
do
    unlzma --keep $INSTANCE.lzma
    set +e  # Temporarily allow errors
    timeout --preserve-status --kill-after=1s 10s \
        ./naxos-xcsp3 $INSTANCE > $SOLUTION
    validate_exit_status $?
    rm $INSTANCE
done

# Check message for unsupported instances
for INSTANCE in $(cat verification/CheckerUnsupportedInstances.txt)
do
    if [ -e $INSTANCE.lzma ]
    then
        unlzma --keep $INSTANCE.lzma
    fi
    (! ./naxos-xcsp3 $INSTANCE > $SOLUTION)
    grep -q "^s UNSUPPORTED$" $SOLUTION
    validate
    if [ -e $INSTANCE.lzma ]
    then
        rm $INSTANCE
    fi
done

# Clean up
rm $SOLUTION
