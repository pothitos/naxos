#! /bin/sh
set -ev
MEM_CHECK="valgrind --leak-check=full --error-exitcode=1"
# Validate 3 Queens empty set of solutions
$MEM_CHECK ./nqueens 3 > solutions.txt
sort solutions.txt | cmp /dev/null
# Validate 4 Queens solutions
$MEM_CHECK ./nqueens 4 > solutions.txt
sort solutions.txt | cmp verification/nqueens_4_sorted_solutions.txt
# Validate 8 Queens solutions
$MEM_CHECK ./nqueens 8 > solutions.txt
sort solutions.txt | cmp verification/nqueens_8_sorted_solutions.txt
# Validate 12 Queens solutions
$MEM_CHECK ./nqueens 12 > solutions.txt
sort solutions.txt | cmp verification/nqueens_12_sorted_solutions.txt
# Validate SEND + MORE = MONEY solution
$MEM_CHECK ./send_more_money > solutions.txt
cmp solutions.txt verification/send_more_money_solution.txt
# Clean up
rm solutions.txt
