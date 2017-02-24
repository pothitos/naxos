// Part of https://github.com/pothitos/naxos

#include <cstdlib>
#include <iostream>
#include <naxos.h>

using namespace naxos;
using namespace std;

int main(int argc, char* argv[])
{
        try {
                int N = (argc > 1) ? atoi(argv[1]) : 3;
                int i, j, sum;
                NsProblemManager pm;
                sum = N * (N * N + 1) / 2;
                // PROBLEM STATEMENT //
                NsIntVarArray Var, *Rows, *Cols, Diag1, Diag2;
                Rows = new NsIntVarArray[N];
                Cols = new NsIntVarArray[N];
                for (i = 0; i < N; ++i)
                        for (j = 0; j < N; ++j)
                                Rows[i].push_back(NsIntVar(pm, 1, N * N));
                for (i = 0; i < N; ++i)
                        for (j = 0; j < N; ++j)
                                Cols[i].push_back(Rows[j][i]);
                for (i = 0; i < N; ++i) {
                        Diag1.push_back(Rows[i][i]);
                        Diag2.push_back(Rows[i][N - i - 1]);
                }
                // put them all together
                for (i = 0; i < N; ++i)
                        for (j = 0; j < N; ++j)
                                Var.push_back(Rows[i][j]);
                pm.add(NsAllDiff(Var));
                for (i = 0; i < N; ++i) {
                        pm.add(NsSum(Rows[i]) == sum);
                        pm.add(NsSum(Cols[i]) == sum);
                }
                pm.add(NsSum(Diag1) == sum);
                pm.add(NsSum(Diag2) == sum);
                // GOALS //
                pm.addGoal(new NsgLabeling(Var));
                // SOLVING //
                if (!pm.nextSolution())
                        return 1;
                pm.printCspParameters();
        } catch (exception& exc) {
                cerr << exc.what() << "\n";
                return 1;
        } catch (...) {
                cerr << "Unknown exception"
                     << "\n";
                return 1;
        }
}
