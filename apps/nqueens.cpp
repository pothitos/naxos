// Part of https://github.com/pothitos/naxos

#include <cstdlib>
#include <iostream>
#include <naxos.h>

using namespace naxos;
using namespace std;

int main(int argc, char* argv[])
{
        try {
                int N = (argc > 1) ? atoi(argv[1]) : 8;
                NsProblemManager pm;
                NsIntVarArray Var, VarPlus, VarMinus;
                for (int i = 0; i < N; ++i) {
                        Var.push_back(NsIntVar(pm, 0, N - 1));
                        VarPlus.push_back(Var[i] + i);
                        VarMinus.push_back(Var[i] - i);
                }
                pm.add(NsAllDiff(Var));
                pm.add(NsAllDiff(VarPlus));
                pm.add(NsAllDiff(VarMinus));
                pm.addGoal(new NsgLabeling(Var));
                while (pm.nextSolution() != false)
                        cout << Var << "\n";
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception\n";
                return 1;
        }
}
