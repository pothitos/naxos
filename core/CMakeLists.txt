add_library(naxos-mini array_constraints.cpp bitset_domain.cpp expressions.cpp
            intvar.cpp problemmanager.cpp var_constraints.cpp)

add_library(naxos array_constraints.cpp bitset_domain.cpp expressions.cpp
            intvar.cpp problemmanager.cpp var_constraints.cpp
            non_mini_solver_expressions.cpp non_mini_solver_constraints.cpp)
