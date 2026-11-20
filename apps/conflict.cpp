// Part of https://github.com/pothitos/naxos

#include <iostream>
#include <naxos.h>

using namespace std;
using namespace naxos;

const char* description[] = {"X < Y", "X + Y == 9", "Y < Z", "X != 4", "Z < X"};

const NsIndex NUM_CONSTRAINTS = 5;

void addConstraint(NsProblemManager& pm, NsIntVar& X, NsIntVar& Y, NsIntVar& Z,
                   const NsIndex c)
{
        switch (c) {
        case 0:
                pm.add(X < Y);
                break;
        case 1:
                pm.add(X + Y == 9);
                break;
        case 2:
                pm.add(Y < Z);
                break;
        case 3:
                pm.add(X != 4);
                break;
        default:
                pm.add(Z < X);
                break;
        }
}

/// Returns true if the constraints in 'subset' can be satisfied together
bool isSatisfiable(const NsDeque<NsIndex>& subset)
{
        NsProblemManager pm;
        NsIntVar X(pm, 0, 9), Y(pm, 0, 9), Z(pm, 0, 9);
        for (NsIndex i = 0; i < subset.size(); ++i)
                addConstraint(pm, X, Y, Z, subset[i]);
        NsIntVarArray vars;
        vars.push_back(X);
        vars.push_back(Y);
        vars.push_back(Z);
        pm.addGoal(new NsgLabeling(vars));
        return pm.nextSolution();
}

int main(void)
{
        try {
                // Explain why the problem has no solution
                NsDeque<NsIndex> conflict =
                    NsMinimalConflict(NUM_CONSTRAINTS, isSatisfiable);
                cout << "Conflict:";
                for (NsIndex i = 0; i < conflict.size(); ++i)
                        cout << (i == 0 ? " " : ", ")
                             << description[conflict[i]];
                cout << "\n";
                // Find the constraints that removed no values while searching
                NsProblemManager pm;
                NsIntVar X(pm, 0, 9), Y(pm, 0, 9);
                NsDeque<NsIndex> indices;
                NsDeque<const char*> constraints;
                indices.push_back(pm.add(X < Y));
                constraints.push_back("X < Y");
                indices.push_back(pm.add(X + Y == 9));
                constraints.push_back("X + Y == 9");
                indices.push_back(pm.add(X <= Y));
                constraints.push_back("X <= Y");
                indices.push_back(pm.add(X != 4));
                constraints.push_back("X != 4");
                NsIntVarArray vars;
                vars.push_back(X);
                vars.push_back(Y);
                pm.addGoal(new NsgLabeling(vars));
                if (pm.nextSolution() != false) {
                        cout << "Solution: X = " << X.value()
                             << ", Y = " << Y.value() << "\n"
                             << "Prunings\tFailures\tConstraint\n";
                        for (NsIndex i = 0; i < indices.size(); ++i)
                                cout << pm.numPrunings(indices[i]) << "\t"
                                     << pm.numFailures(indices[i]) << "\t"
                                     << constraints[i] << "\n";
                }
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception\n";
                return 1;
        }
}
