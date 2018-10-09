#!/bin/sh -ev
# Set default values, if variables are unset
METHOD=${METHOD-"DBS"}
DATASET=${DATASET-"comp01.ctt"}
# If Valgrind is installed, do memory check too
if command -v valgrind
then
    MEM_CHECK="valgrind -q --leak-check=full --error-exitcode=1"
fi
# Calculate the available time for searching a solution
cd datasets
AVAILABLE_SECONDS=$(yes | ./benchmark_my_linux_machine | \
                    grep "seconds" | grep -o "[[:digit:]]*")
cd -
# Set the search method
sed -i.bak "s/^\(searchMethod\) .*/\1 $METHOD/" optionsFile
# Set the available time
sed -i "s/^\(timeLimit\) .*/\1 $AVAILABLE_SECONDS/" optionsFile
if [ -z "$METHOD_LOCAL_SEARCH" ]
then
    sed -i "s/^\(timeLimitDirectMethodRound\) .*/\1 $AVAILABLE_SECONDS/" optionsFile
else
    sed -i "s/^\(timeLimitDirectMethodRound\) .*/\1 $(($AVAILABLE_SECONDS / 4))/" optionsFile
    sed -i "s/^\(timeLimitIndirectMethodRound\) .*/\1 $(($AVAILABLE_SECONDS / 4))/" optionsFile
    sed -i "s/^\(metaSearchMethod\) .*/\1 $METHOD_LOCAL_SEARCH/" optionsFile
fi
# Execute the curriculum based course timetabling solver
$MEM_CHECK ./itc_solver datasets/$DATASET solution.txt -options optionsFile > progress.txt
cat progress.txt
# Restore the original file
mv optionsFile.bak optionsFile
# Compile the official validator
g++ verification/validator.cc -o verification/validator
# Validate the last solution found
verification/validator datasets/$DATASET solution.txt > validation.txt
# Calculate the constraint categories were no hard constraints were violated
CATEGORIES=$(grep -c "^Violations of [[:alpha:]]* (hard) : 0$" validation.txt)
# The constraint categories are four
test $CATEGORIES -eq 4
# Get the solution cost calculated by the validator
COST_VALIDATOR=$(grep -o "Cost = [[:digit:]]*" validation.txt | grep -o "[[:digit:]]*")
# Get the solution cost calculated by the solver
COST_SOLVER=$(grep -o "cost [[:digit:]]*" progress.txt | grep -o "[[:digit:]]*" | tail -1)
# The two costs should be equal
test $COST_VALIDATOR -eq $COST_SOLVER
# Clean up
rm progress.txt solution.txt validation.txt
