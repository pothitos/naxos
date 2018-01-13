#include <amorgos.h>
#include <naxos.h>

#include <cassert>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace naxos;

// value ordering heuristic for n-queens: Select values in the middle first
class ValHeurNqueensMiddle : public ValueHeuristic {
        int N; // number of queens
    public:
        ValHeurNqueensMiddle(int nInit) : N(nInit)
        {
        }
        naxos::NsInt select(const naxos::NsIntVar& V)
        {
                int mid = N / 2;
                int i;
                if (V.contains(mid))
                        return mid;
                // find value closest to the middle
                for (i = 1;; ++i) {
                        assert(mid - i >= 0);
                        if (V.contains(mid - i))
                                return mid - i;
                        assert(mid + i < N);
                        if (V.contains(mid + i))
                                return mid + i;
                }
                return 0; // prevent warning
        }
};

int main(int argc, char* argv[])
{
        try {
                int N = (argc > 1) ? atoi(argv[1]) : 8;
                NsProblemManager pm;
                // PROBLEM STATEMENT //
                NsIntVarArray Var, VarPlus, VarMinus;
                for (int i = 0; i < N; ++i) {
                        Var.push_back(NsIntVar(pm, 0, N - 1));
                        VarPlus.push_back(Var[i] + i);
                        VarMinus.push_back(Var[i] - i);
                }
                pm.add(NsAllDiff(Var));
                pm.add(NsAllDiff(VarPlus));
                pm.add(NsAllDiff(VarMinus));
                // GOALS //
                // pm.addGoal(new goalLan(Var, 100, new
                // ValHeurNqueensMiddle(N)));
                pm.addGoal(new AmIsampStepping(Var, 100));
                // pm.addGoal(new goalFnsLabeling(Var, fSample, new VarHeurMRV,
                // new ValHeurNqueensMiddle(N)));
                // SOLVING //
                while (pm.nextSolution() != false)
                        cout << "Solution: " << Var << "\n";
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
        }
}
