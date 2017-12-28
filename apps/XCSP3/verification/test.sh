#! /bin/sh

install_validator() {
    # Install XCSP3 Checker
    cd verification/
    if [ ! -e XCSP3-Java-Tools ]
    then
        git clone https://github.com/xcsp3team/XCSP3-Java-Tools.git
    fi
    cd XCSP3-Java-Tools/src/main/java/
    git pull --ff-only
    if [ ! -e org/xcsp/checker/SolutionChecker.class ]
    then
        javac -classpath ../../../lib/javax.json-1.0.4.jar -sourcepath . \
            org/xcsp/checker/SolutionChecker.java
    fi
    cd ../../../../../
}

validate() {
    # Solution validation tool
    VALIDATOR="java -classpath verification/XCSP3-Java-Tools/src/main/java/
               org.xcsp.checker.SolutionChecker -cm"

    COST=$(grep "^o -\?[[:digit:]]\+$" $SOLUTION | tail -1 |
           grep -o -- "-\?[[:digit:]]\+" || true)
    VALIDATION=$($VALIDATOR $INSTANCE $SOLUTION 2>&1)
    ! VALIDATION=$(echo "$VALIDATION" |
                   grep -v "^Picked up _JAVA_OPTIONS: \|^ROOT\|^$")
    if echo "$VALIDATION" | grep -q \
                            "^ERROR: the instantiation cannot be checked "
    then
        echo "$INSTANCE: Cannot check the instantiation"
    elif [ "$VALIDATION" = "" ]
    then
        echo
        echo "$INSTANCE:"
        cat $SOLUTION
    elif [ "$VALIDATION" != "OK	$COST" ]
    then
        echo
        echo "Wrong solution for $INSTANCE:"
        cat $SOLUTION
        echo "$VALIDATION"
        exit 1
    fi
}

set -ev

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
MEM_CHECK="valgrind -q"

# Temporary file
SOLUTION="/tmp/instance.sol"

# XCSP3 Parser's Traveling Salesman Problem instance
INSTANCE="parser/src/XCSP3-CPP-Parser/instances/tsp-25-843.xml"
timeout --preserve-status --kill-after=1s 10s \
    $MEM_CHECK ./naxos-xcsp3 $INSTANCE > $SOLUTION
validate

# XCSP3 Parser's Constraint Optimisation (COP) instance
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
    timeout --preserve-status --kill-after=1s 10s \
        ./naxos-xcsp3 $INSTANCE > $SOLUTION
    validate
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
