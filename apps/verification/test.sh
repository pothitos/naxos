#! /bin/sh
set -ev
# Validate 3 Queens empty set of solutions
./nqueens 3 | cmp /dev/null
# Validate 4 Queens solutions
./nqueens 4 | sort | cmp verification/nqueens_4_sorted_solutions.txt
# Validate 8 Queens solutions
./nqueens | sort | cmp verification/nqueens_8_sorted_solutions.txt
# Validate 12 Queens solutions
./nqueens 12 | sort | cmp verification/nqueens_12_sorted_solutions.txt
# Validate SEND + MORE = MONEY solution
./send_more_money | cmp verification/send_more_money_solution.txt
