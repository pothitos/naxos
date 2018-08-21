#!/bin/sh

get_solution_cost() {
    grep "^o -\?[[:digit:]]\+$" $SOLUTION | tail -1 | \
        grep -o -- "-\?[[:digit:]]\+" || true
}

validate() {
    # Solution validation tool
    VALIDATOR="java -classpath
               verification/XCSP3-Java-Tools/build/libs/xcsp3-tools-*.jar
               org.xcsp.parser.callbacks.SolutionChecker -cm"

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
    elif [ "$VALIDATION" != "OK	$(get_solution_cost)" ]
    then
        echo
        echo "Wrong solution for $INSTANCE:"
        cat $SOLUTION
        echo "$VALIDATION"
        exit 1
    fi
}

validate_exit_status() {
    set -e
    STATUS="$1"
    if [ -z "$STATUS" ]
    then
        echo "validate_exit_status: Missing status argument!" >&2
        exit 1
    fi
    if [ $STATUS -eq 137 ]
    then
        #     Killed
        echo "       $INSTANCE before normal termination"
        if [ -s $SOLUTION ]
        then
            validate
        fi
    else
        if [ $STATUS -ne 0 ]
        then
            echo "$INSTANCE: Unexpected exit status $STATUS"
            if [ -s $SOLUTION ]
            then
                cat $SOLUTION
            fi
            exit $STATUS
        fi
        validate
    fi
}
